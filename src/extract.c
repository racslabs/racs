#include "extract.h"

static int compare_paths(const void* path1, const void* path2);
static file_list_t* file_list_create();
static file_list_t* get_sorted_file_list(const char* path);
static void file_list_destroy(file_list_t* list);
static void file_list_add(file_list_t* list, const char* file_path);
static void list_files_recursive(file_list_t* list, const char* path);
static void file_list_sort(file_list_t* list);
static pcm_buffer_t* pcm_buffer_create(uint32_t channels, uint32_t sample_rate, uint32_t bits_per_sample);
static void pcm_buffer_append(pcm_buffer_t* pbuf, pcm_block_t* block);
static void pcm_block_destroy(pcm_block_t* block);
static uint64_t time_partitioned_path_to_timestamp(const char* path);
static char* resolve_shared_path(const char* path1, const char* path2);
static char* get_path_from_time_range(uint64_t begin_timestamp, uint64_t end_timestamp);
static uint8_t* get_data_from_cache_or_sstable(cache_t* cache, uint64_t stream_id, uint64_t timestamp, const char* file_path);
static flac_encoded_blocks_t* extract_flac_encoded_blocks(cache_t* cache, uint64_t stream_id, uint64_t begin_timestamp, uint64_t end_timestamp);
static void process_sstable_data(flac_encoded_blocks_t* blocks, uint64_t stream_id, uint64_t begin_timestamp, uint64_t end_timestamp, uint8_t* data);
static uint8_t* serialize_pcm_data(const pcm_buffer_t* pbuf);
static uint64_t next_power_of_two(uint64_t n);

pcm_buffer_t* extract_pcm_data(cache_t* cache, uint64_t stream_id, uint64_t begin_timestamp, uint64_t end_timestamp) {
    if (!cache) {
        return NULL;
    }

    flac_encoded_blocks_t* blocks = extract_flac_encoded_blocks(cache, stream_id, begin_timestamp, end_timestamp);
    if (!blocks->num_blocks) {
        auxts_flac_encoded_blocks_destroy(blocks);
        return NULL;
    }

    flac_encoded_block_t* flac_block = blocks->blocks[0];
    pcm_block_t* pcm_block = auxts_decode_flac_block(flac_block);
    pcm_buffer_t* pbuf = pcm_buffer_create(pcm_block->channels, pcm_block->sample_rate, pcm_block->bits_per_sample);

    pcm_buffer_append(pbuf, pcm_block);
    pcm_block_destroy(pcm_block);

    for (int i = 1; i < blocks->num_blocks; ++i) {
        flac_block = blocks->blocks[i];
        pcm_block = auxts_decode_flac_block(flac_block);

        pcm_buffer_append(pbuf, pcm_block);
        pcm_block_destroy(pcm_block);
    }

    auxts_flac_encoded_blocks_destroy(blocks);

    return pbuf;
}

void pcm_buffer_destroy(pcm_buffer_t* pbuf) {
    if (!pbuf) {
        return;
    }

    for (int channel = 0; channel < pbuf->channels; ++channel) {
        free(pbuf->data[channel]);
    }

    free(pbuf->data);
    free(pbuf);
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
    ts.tv_nsec = milliseconds * AUXTS_NANOSECONDS_PER_MILLISECOND;

    return auxts_ts_to_milliseconds(&ts);
}

int compare_paths(const void* path1, const void* path2) {
    return strcmp(*(const char**)path1, *(const char**) path2);
}

file_list_t* file_list_create() {
    file_list_t* list = malloc(sizeof(file_list_t));
    if (!list) {
        perror("Failed to allocate file_list_t");
        file_list_destroy(list);
        return NULL;
    }

    list->num_files = 0;
    list->max_num_files = AUXTS_INITIAL_FILE_LIST_CAPACITY;

    list->files = malloc(AUXTS_INITIAL_FILE_LIST_CAPACITY * sizeof(char*));
    if (!list->files) {
        perror("Failed to allocate file paths to file_list_t");
        file_list_destroy(list);
        return NULL;
    }

    return list;
}

void file_list_add(file_list_t* list, const char* file_path) {
    if (list->num_files == list->max_num_files) {
        list->max_num_files = 1 << list->max_num_files;

        char** files = realloc(list->files, list->max_num_files * sizeof(char*));
        if (!files) {
            perror("Error reallocating file paths to file_list_t");
            return;
        }

        list->files = files;
    }

    list->files[list->num_files] = strdup(file_path);
    ++list->num_files;
}

void list_files_recursive(file_list_t* list, const char* path) {
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

void file_list_destroy(file_list_t* list) {
    for (int i = 0; i < list->num_files; ++i) {
        free(list->files[i]);
    }

    free(list->files);
    free(list);
}

void file_list_sort(file_list_t* list) {
    qsort(list->files, list->num_files, sizeof(char*), compare_paths);
}

char* get_path_from_time_range(uint64_t begin_timestamp, uint64_t end_timestamp) {
    char path1[255], path2[255];

    auxts_get_time_partitioned_path(begin_timestamp, path1);
    auxts_get_time_partitioned_path(end_timestamp, path2);

    return resolve_shared_path(path1, path2);
}

file_list_t* get_sorted_file_list(const char* path) {
    file_list_t* list = file_list_create();
    list_files_recursive(list, path);
    file_list_sort(list);

    return list;
}

uint8_t* get_data_from_cache_or_sstable(cache_t* cache, uint64_t stream_id, uint64_t timestamp, const char* file_path) {
    uint64_t key[2] = {stream_id, timestamp};
    uint8_t* data = auxts_cache_get(cache, key);

    if (!data) {
        sstable_t* sstable = auxts_read_sstable_index_entries(file_path);
        if (!sstable) {
            return NULL;
        }

        data = sstable->data;
        auxts_sstable_destroy_except_data(sstable);

        auxts_cache_put(cache, key, data);
    }

    return data;
}

void process_sstable_data(flac_encoded_blocks_t* blocks, uint64_t stream_id, uint64_t begin_timestamp, uint64_t end_timestamp, uint8_t* data) {
    size_t size;
    memcpy(&size, data, sizeof(size_t));
    size = auxts_swap64_if_big_endian(size);

    sstable_t* sstable = auxts_read_sstable_index_entries_in_memory(data, size);
    if (!sstable) {
        return;
    }

    for (int j = 0; j < sstable->num_entries; ++j) {
        size_t offset = sstable->index_entries[j].offset;

        memtable_entry_t* entry = auxts_read_memtable_entry(data, offset);
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

flac_encoded_blocks_t* extract_flac_encoded_blocks(cache_t* cache, uint64_t stream_id, uint64_t begin_timestamp, uint64_t end_timestamp) {
    char* path = get_path_from_time_range(begin_timestamp, end_timestamp);
    file_list_t* list = get_sorted_file_list(path);

    flac_encoded_blocks_t* blocks = auxts_flac_encoded_blocks_create();
    if (!blocks) {
        free(path);
        file_list_destroy(list);
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

pcm_buffer_t* pcm_buffer_create(uint32_t channels, uint32_t sample_rate, uint32_t bits_per_sample) {
    pcm_buffer_t* pbuf = malloc(sizeof(pcm_buffer_t));
    if (!pbuf) {
        perror("Failed to allocate pcm_buffer_t");
        return NULL;
    }

    pbuf->num_samples = 0;
    pbuf->channels = channels;
    pbuf->sample_rate = sample_rate;
    pbuf->bits_per_sample = bits_per_sample;
    pbuf->max_num_samples = AUXTS_INITIAL_PCM_BUFFER_CAPACITY;

    pbuf->data = malloc(channels * sizeof(int32_t*));
    if (!pbuf->data) {
        perror("Failed to allocate data to pcm_buffer_t");
        free(pbuf);
        return NULL;
    }

    for (int channel = 0; channel < channels; ++channel) {
        pbuf->data[channel] = malloc(AUXTS_INITIAL_PCM_BUFFER_CAPACITY * sizeof(int32_t));
    }

    return pbuf;
}

void pcm_block_destroy(pcm_block_t* block) {
    for (int channel = 0; channel < block->channels; ++channel) {
        free(block->data[channel]);
    }

    free(block->data);
    free(block);
}

void pcm_buffer_append(pcm_buffer_t* pbuf, pcm_block_t* block) {
    if (!pbuf || !block) {
        return;
    }

    size_t num_samples = block->total_samples + pbuf->num_samples;

    if (num_samples > pbuf->max_num_samples) {
        pbuf->max_num_samples = next_power_of_two(num_samples);

        for (int channel = 0; channel < pbuf->channels; ++channel) {
            int32_t* audio_data = realloc(pbuf->data[channel], pbuf->max_num_samples * sizeof(int32_t));
            if (!audio_data) {
                return;
            }

            pbuf->data[channel] = audio_data;
        }
    }

    for (int channel = 0; channel < pbuf->channels; ++channel) {
        memcpy(pbuf->data[channel] + pbuf->num_samples, block->data[channel], block->total_samples * sizeof(int32_t));
    }

    pbuf->num_samples = num_samples;
}

uint8_t* serialize_pcm_data(const pcm_buffer_t* pbuf) {
    uint8_t* samples = malloc(pbuf->num_samples * pbuf->channels * sizeof(int32_t));
    if (!samples) {
        perror("Failed to allocate samples");
        return NULL;
    }

    for (int channel = 0; channel < pbuf->channels; ++channel) {
        #ifdef __ARM_NEON__
            auxts_bulk_swap32_if_big_endian(pbuf->data[channel], pbuf->num_samples);
        #endif
        memcpy(samples + (channel * pbuf->num_samples), pbuf->data[channel], pbuf->num_samples * sizeof(int32_t));
    }

    return samples;
}

auxts_result_t* serialize_pcm_buffer(const pcm_buffer_t* pbuf) {
    msgpack_sbuffer sbuf;
    msgpack_sbuffer_init(&sbuf);

    msgpack_packer pk;
    msgpack_packer_init(&pk, &sbuf, msgpack_sbuffer_write);

    msgpack_pack_map(&pk, 5);

    msgpack_pack_str(&pk, 6);
    msgpack_pack_str_body(&pk, "status", 6);

    msgpack_pack_str(&pk, 2);
    msgpack_pack_str_body(&pk, "OK", 2);

    msgpack_pack_str(&pk, 8);
    msgpack_pack_str_body(&pk, "channels", 8);
    msgpack_pack_uint32(&pk, pbuf->channels);

    msgpack_pack_str(&pk, 11);
    msgpack_pack_str_body(&pk, "sample_rate", 11);
    msgpack_pack_uint32(&pk, pbuf->sample_rate);

    msgpack_pack_str(&pk, 15);
    msgpack_pack_str_body(&pk, "bits_per_sample", 15);
    msgpack_pack_uint32(&pk, pbuf->bits_per_sample);

    msgpack_pack_str(&pk, 15);
    msgpack_pack_str_body(&pk, "num_samples", 15);
    msgpack_pack_uint32(&pk, pbuf->bits_per_sample);

    msgpack_pack_str(&pk, 8);
    msgpack_pack_str_body(&pk, "pcm_data", 8);

    uint8_t* data = serialize_pcm_data(pbuf);
    if (!data) {
        msgpack_pack_nil(&pk);
    } else {
        size_t size = pbuf->num_samples * pbuf->channels * sizeof(int32_t);
        msgpack_pack_bin(&pk, size);
        msgpack_pack_bin_body(&pk, pbuf->data, size);
    }

    auxts_result_t* result = malloc(sizeof(auxts_result_t));
    if (!result) {
        perror("Failed to allocate auxts_result_t");
        msgpack_sbuffer_destroy(&sbuf);
        msgpack_packer_free(&pk);
        return NULL;
    }

    result->data = malloc(sbuf.size);
    if (!result->data) {
        perror("Failed to allocate data to auxts_result_t");
        msgpack_sbuffer_destroy(&sbuf);
        msgpack_packer_free(&pk);
        free(result);
        return NULL;
    }

    memcpy(result->data, sbuf.data, sbuf.size);
    result->size = sbuf.size;

    return result;
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
