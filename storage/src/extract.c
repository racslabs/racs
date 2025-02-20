#include "extract.h"

static int compare_paths(const void* path1, const void* path2);
static FileList* file_list_create();
static FileList* get_sorted_file_list(const char* path);
static void file_list_destroy(FileList* list);
static void file_list_add(FileList* list, const char* file_path);
static void list_files_recursive(FileList* list, const char* path);
static void file_list_sort(FileList* list);
static PcmBuffer* pcm_buffer_create(uint32_t channels, uint32_t sample_rate, uint32_t bits_per_sample);
static void pcm_buffer_append(PcmBuffer* buffer, PcmBlock* block);
static void pcm_block_destroy(PcmBlock* block);
static uint64_t time_partitioned_path_to_timestamp(const char* path);
static char* resolve_shared_path(const char* path1, const char* path2);
static char* get_path_from_time_range(uint64_t begin_timestamp, uint64_t end_timestamp);
static uint8_t* get_data_from_cache_or_sstable(LRUCache* cache, uint64_t stream_id, uint64_t timestamp, const char* file_path);
static FlacEncodedBlocks* extract_flac_encoded_blocks(LRUCache* cache, uint64_t stream_id, uint64_t begin_timestamp, uint64_t end_timestamp);
static void process_sstable_data(FlacEncodedBlocks* blocks, uint64_t stream_id, uint64_t begin_timestamp, uint64_t end_timestamp, uint8_t* data);
static uint64_t next_power_of_two(uint64_t n);

PcmBuffer* auxts_extract_pcm_data(LRUCache* cache, uint64_t stream_id, uint64_t begin_timestamp, uint64_t end_timestamp) {
    if (!cache) {
        return NULL;
    }

    FlacEncodedBlocks* blocks = extract_flac_encoded_blocks(cache, stream_id, begin_timestamp, end_timestamp);
    if (!blocks->num_blocks) {
        auxts_flac_encoded_blocks_destroy(blocks);
        return NULL;
    }

    FlacEncodedBlock* flac_block = blocks->blocks[0];
    PcmBlock* pcm_block = auxts_decode_flac_block(flac_block);
    PcmBuffer* buffer = pcm_buffer_create(pcm_block->channels, pcm_block->sample_rate, pcm_block->bits_per_sample);

    pcm_buffer_append(buffer, pcm_block);
    pcm_block_destroy(pcm_block);

    for (int i = 1; i < blocks->num_blocks; ++i) {
        flac_block = blocks->blocks[i];
        pcm_block = auxts_decode_flac_block(flac_block);

        pcm_buffer_append(buffer, pcm_block);
        pcm_block_destroy(pcm_block);
    }

    auxts_flac_encoded_blocks_destroy(blocks);

    return buffer;
}

void auxts_pcm_buffer_destroy(PcmBuffer* buffer) {
    if (!buffer) {
        return;
    }

    for (int channel = 0; channel < buffer->channels; ++channel) {
        free(buffer->data[channel]);
    }

    free(buffer->data);
    free(buffer);
}

char* resolve_shared_path(const char* path1, const char* path2) {
    if (!path1 || !path2) {
        perror("Paths cannot be null");
        return NULL;
    }

    size_t len1 = strlen(path1);
    size_t len2 = strlen(path2);
    size_t len = len1 < len2 ? len1 : len2;

    size_t i = 0;
    for ( ; i < len; i++) {
        if (path1[i] != path2[i]) {
            break;
        }
    }

    while (i > 1 && path1[i - 1] != '/') --i;
    --i;

    char* shared_path = malloc(i + 1);
    strncpy(shared_path, path1, i);
    shared_path[i] = '\0';

    return shared_path;
}

uint64_t time_partitioned_path_to_timestamp(const char* path) {
    struct tm info = {0};
    struct timespec ts;
    long milliseconds = 0;

    int ret = sscanf(path, ".data/%4d/%2d/%2d/%2d/%2d/%2d/%3ld.df",
                     &info.tm_year, &info.tm_mon, &info.tm_mday,
                     &info.tm_hour, &info.tm_min, &info.tm_sec, &milliseconds);

    if (ret != 7) {
        fprintf(stderr, "Invalid format: expected 7 values, got %d\n", ret);
        return -1;
    }

    if (milliseconds < 0 || milliseconds > 999) {
        fprintf(stderr, "Invalid milliseconds value: %ld\n", milliseconds);
        return -1;
    }

    info.tm_year -= 1900;
    info.tm_mon -= 1;

    time_t t = timegm(&info);
    if (t == -1) {
        perror("timegm failed");
        return -1;
    }

    ts.tv_sec = t;
    ts.tv_nsec = milliseconds * AUXTS__NANOSECONDS_PER_MILLISECOND;

    return AUXTS__ts_to_milliseconds(&ts);
}

int compare_paths(const void* path1, const void* path2) {
    return strcmp(*(const char**)path1, *(const char**) path2);
}

FileList* file_list_create() {
    FileList* list = malloc(sizeof(FileList));
    if (!list) {
        perror("Failed to allocate FileList");
        file_list_destroy(list);
        return NULL;
    }

    list->num_files = 0;
    list->max_num_files = AUXTS_INITIAL_FILE_LIST_CAPACITY;

    list->files = malloc(AUXTS_INITIAL_FILE_LIST_CAPACITY * sizeof(char*));
    if (!list->files) {
        perror("Failed to allocate file paths to FileList");
        file_list_destroy(list);
        return NULL;
    }

    return list;
}

void file_list_add(FileList* list, const char* file_path) {
    if (list->num_files == list->max_num_files) {
        list->max_num_files = 1 << list->max_num_files;

        char** files = realloc(list->files, list->max_num_files * sizeof(char*));
        if (!files) {
            perror("Error reallocating file paths to FileList");
            return;
        }

        list->files = files;
    }

    list->files[list->num_files] = strdup(file_path);
    ++list->num_files;
}

void list_files_recursive(FileList* list, const char* path) {
    DIR* dir = opendir(path);
    if (!dir) {
        perror("Failed to open directory");
        closedir(dir);
        return;
    }

    struct dirent* entry;
    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        char file_path[255];
        snprintf(file_path, 255, "%s/%s", path, entry->d_name);

        if (entry->d_type == DT_DIR) {
            list_files_recursive(list, file_path);
        } else if (entry->d_type == DT_REG) {
            file_list_add(list, file_path);
        }
    }

    closedir(dir);
}

void file_list_destroy(FileList* list) {
    for (int i = 0; i < list->num_files; ++i) {
        free(list->files[i]);
    }

    free(list->files);
    free(list);
}

void file_list_sort(FileList* list) {
    qsort(list->files, list->num_files, sizeof(char*), compare_paths);
}

char* get_path_from_time_range(uint64_t begin_timestamp, uint64_t end_timestamp) {
    char path1[255], path2[255];

    auxts_get_time_partitioned_path(begin_timestamp, path1);
    auxts_get_time_partitioned_path(end_timestamp, path2);

    return resolve_shared_path(path1, path2);
}

FileList* get_sorted_file_list(const char* path) {
    FileList* list = file_list_create();
    list_files_recursive(list, path);
    file_list_sort(list);

    return list;
}

uint8_t* get_data_from_cache_or_sstable(LRUCache* cache, uint64_t stream_id, uint64_t timestamp, const char* file_path) {
    uint64_t key[2] = {stream_id, timestamp};
    uint8_t* data = auxts_lru_cache_get(cache, key);

    if (!data) {
        SSTable* sstable = auxts_read_sstable_index_entries(file_path);
        if (!sstable) {
            return NULL;
        }

        data = sstable->data;
        auxts_sstable_destroy_except_data(sstable);

        auxts_lru_cache_put(cache, key, data);
    }

    return data;
}

void process_sstable_data(FlacEncodedBlocks* blocks, uint64_t stream_id, uint64_t begin_timestamp, uint64_t end_timestamp, uint8_t* data) {
    size_t size;
    memcpy(&size, data, sizeof(size_t));
    size = auxts_swap64_if_big_endian(size);

    SSTable* sstable = auxts_read_sstable_index_entries_in_memory(data, size);
    if (!sstable) {
        return;
    }

    for (int j = 0; j < sstable->num_entries; ++j) {
        size_t offset = sstable->index_entries[j].offset;

        MemtableEntry* entry = auxts_read_memtable_entry(data, offset);
        if (!entry) {
            return;
        }

        uint64_t timestamp = entry->key[1];

        if (entry->key[0] == stream_id && timestamp >= begin_timestamp && timestamp <= end_timestamp) {
            auxts_flac_encoded_blocks_append(blocks, entry->block, entry->block_size);
        } else {
            free(entry->block);
        }

        free(entry);
    }

    auxts_sstable_destroy_except_data(sstable);
}

FlacEncodedBlocks* extract_flac_encoded_blocks(LRUCache* cache, uint64_t stream_id, uint64_t begin_timestamp, uint64_t end_timestamp) {
    char* path = get_path_from_time_range(begin_timestamp, end_timestamp);
    FileList* list = get_sorted_file_list(path);

    FlacEncodedBlocks* blocks = auxts_flac_encoded_blocks_create();
    if (!blocks) {
        return NULL;
    }

    for (int i = 0; i < list->num_files; ++i) {
        char* file_path = list->files[i];

        uint64_t timestamp = time_partitioned_path_to_timestamp(file_path);
        if (timestamp >= begin_timestamp && timestamp <= end_timestamp) {
            uint8_t* buffer = get_data_from_cache_or_sstable(cache, stream_id, timestamp, file_path);
            process_sstable_data(blocks, stream_id, begin_timestamp, end_timestamp, buffer);
        }
    }

    free(path);
    file_list_destroy(list);

    return blocks;
}

PcmBuffer* pcm_buffer_create(uint32_t channels, uint32_t sample_rate, uint32_t bits_per_sample) {
    PcmBuffer* buffer = malloc(sizeof(PcmBuffer));
    if (!buffer) {
        perror("Failed to allocate PcmBuffer");
        return NULL;
    }

    buffer->num_samples = 0;
    buffer->channels = channels;
    buffer->sample_rate = sample_rate;
    buffer->bits_per_sample = bits_per_sample;
    buffer->max_num_samples = AUXTS_INITIAL_PCM_BUFFER_CAPACITY;

    buffer->data = malloc(channels * sizeof(int32_t*));
    if (!buffer->data) {
        perror("Failed to allocate data to PcmBuffer");
        free(buffer);
        return NULL;
    }

    for (int channel = 0; channel < channels; ++channel) {
        buffer->data[channel] = malloc(AUXTS_INITIAL_PCM_BUFFER_CAPACITY * sizeof(int32_t));
    }

    return buffer;
}

void pcm_block_destroy(PcmBlock* block) {
    for (int channel = 0; channel < block->channels; ++channel) {
        free(block->data[channel]);
    }

    free(block->data);
    free(block);
}

void pcm_buffer_append(PcmBuffer* buffer, PcmBlock* block) {
    if (!buffer || !block) {
        return;
    }

    size_t num_samples = block->total_samples + buffer->num_samples;

    if (num_samples > buffer->max_num_samples) {
        buffer->max_num_samples = next_power_of_two(num_samples);

        for (int channel = 0; channel < buffer->channels; ++channel) {
            int32_t* audio_data = realloc(buffer->data[channel], buffer->max_num_samples * sizeof(int32_t));
            if (!audio_data) {
                return;
            }

            buffer->data[channel] = audio_data;
        }
    }

    for (int channel = 0; channel < buffer->channels; ++channel) {
        memcpy(buffer->data[channel] + buffer->num_samples, block->data[channel], block->total_samples * sizeof(int32_t));
    }

    buffer->num_samples = num_samples;
}

uint64_t next_power_of_two(uint64_t n) {
    n--;
    n |= n >> 1;
    n |= n >> 2;
    n |= n >> 4;
    n |= n >> 8;
    n |= n >> 16;
    n++;

    return n;
}

int test_extract() {
    LRUCache* cache = auxts_lru_cache_create(2);
    PcmBuffer* buffer = auxts_extract_pcm_data(cache, 8870522515535040796, 1739141512213, 1739141512214);

    printf("pcm buffer size: %zu\n", buffer->num_samples);
    auxts_pcm_buffer_destroy(buffer);

    auxts_lru_cache_destroy(cache);

    return 0;
}
