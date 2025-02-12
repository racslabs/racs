#include "extract.h"

static char* resolve_shared_path(const char* path1, const char* path2);
static int compare_paths(const void* path1, const void* path2);
static AUXTS__FileList* FileList_construct();
static void FileList_destroy(AUXTS__FileList* list);
static void FileList_add_file(AUXTS__FileList* list, const char* file_path);
static void list_files_recursive(AUXTS__FileList* list, const char* path);
static void FileList_sort(AUXTS__FileList* list);
static uint64_t time_partitioned_path_to_ts(const char* path);
static char* get_path_from_time_range(uint64_t begin_ts, uint64_t end_ts);
static uint8_t* get_buffer_from_cache_or_sstable(AUXTS__LRUCache* cache, uint64_t stream_id, uint64_t curr_ts, const char* file_path);
static AUXTS__FileList* get_sorted_file_list(const char* path);
static AUXTS__FlacEncodedStream* FlacEncodedStream_construct();
static void FlacEncodedStream_append(AUXTS__FlacEncodedStream* blocks, uint8_t* block_data, uint16_t size);
static AUXTS__FlacEncodedStream* extract_flac_encoded_blocks(AUXTS__LRUCache* cache, uint64_t stream_id, uint64_t begin_ts, uint64_t end_ts);
static void process_sstable_buffer(AUXTS__FlacEncodedStream* stream, uint64_t stream_id, uint8_t* buffer);

char* resolve_shared_path(const char* path1, const char* path2) {
    if (!path1 || !path2) {
        perror("Paths cannot be null");
        exit(EXIT_FAILURE);
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

uint64_t time_partitioned_path_to_ts(const char* path) {
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

AUXTS__FileList* FileList_construct() {
    AUXTS__FileList* list = malloc(sizeof(AUXTS__FileList));
    if (!list) {
        perror("Failed to allocate AUXTS__FileList");
        exit(EXIT_FAILURE);
    }

    list->count = 0;
    list->capacity = AUXTS__INITIAL_FILE_LIST_CAPACITY;
    list->files = malloc(AUXTS__INITIAL_FILE_LIST_CAPACITY * sizeof(char*));

    return list;
}

void FileList_add_file(AUXTS__FileList* list, const char* file_path) {
    if (list->count == list->capacity) {
        list->capacity = 1 << list->capacity;
        list->files = realloc(list->files, list->capacity * sizeof(char*));

        if (!list->files) {
            perror("Error reallocating list to AUXTS__FileList");
            exit(EXIT_FAILURE);
        }
    }

    list->files[list->count] = strdup(file_path);
    ++list->count;
}

void list_files_recursive(AUXTS__FileList* list, const char* path) {
    DIR* dir = opendir(path);
    if (!dir) {
        perror("Failed to open directory");
        exit(EXIT_FAILURE);
    }

    struct dirent* entry;
    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        char file_path[AUXTS__MAX_PATH_SIZE];
        snprintf(file_path, AUXTS__MAX_PATH_SIZE, "%s/%s", path, entry->d_name);

        if (entry->d_type == DT_DIR) {
            list_files_recursive(list, file_path);
        } else if (entry->d_type == DT_REG) {
            FileList_add_file(list, file_path);
        }
    }

    closedir(dir);
}

void FileList_destroy(AUXTS__FileList* list) {
    for (int i = 0; i < list->count; ++i) {
        free(list->files[i]);
    }

    free(list->files);
    free(list);
}

void FileList_sort(AUXTS__FileList* list) {
    qsort(list->files, list->count, sizeof(char*), compare_paths);
}

AUXTS__FlacEncodedStream* FlacEncodedStream_construct() {
    AUXTS__FlacEncodedStream* stream = malloc(sizeof(AUXTS__FlacEncodedStream));
    if (!stream) {
        perror("Failed to allocate AUXTS__FlacEncodedStream");
        exit(EXIT_FAILURE);
    }

    stream->capacity = AUXTS__INITIAL_BLOCK_STREAM_CAPACITY;
    stream->size = 0;

    stream->blocks = malloc(stream->capacity * sizeof(AUXTS__FlacEncodedBlock));
    if (!stream->blocks) {
        perror("Failed to allocate blocks to AUXTS__FlacEncodedStream");
        exit(EXIT_FAILURE);
    }

    return stream;
}

void FlacEncodedStream_append(AUXTS__FlacEncodedStream* blocks, uint8_t* block_data, uint16_t size) {
    if (blocks->size == blocks->capacity) {
        blocks->capacity = 1 << blocks->capacity;
        blocks->blocks = realloc(blocks->blocks, blocks->capacity * sizeof(AUXTS__FlacEncodedBlock));

        if (!blocks->blocks) {
            perror("Error reallocating blocks to AUXTS__FlacEncodedStream");
            exit(EXIT_FAILURE);
        }
    }

    AUXTS__FlacEncodedBlock* block = malloc(sizeof(AUXTS__FlacEncodedBlock));
    if (!block) {
        perror("Failed to allocate AUXTS__FlacEncodedBlock");
        exit(EXIT_FAILURE);
    }

    block->data = block_data;
    block->size = size;

    blocks->blocks[blocks->size] = block;
    ++blocks->size;
}

char* get_path_from_time_range(uint64_t begin_ts, uint64_t end_ts) {
    char path1[AUXTS__MAX_PATH_SIZE], path2[AUXTS__MAX_PATH_SIZE];

    AUXTS__get_time_partitioned_path(begin_ts, path1);
    AUXTS__get_time_partitioned_path(end_ts, path2);

    return resolve_shared_path(path1, path2);
}

AUXTS__FileList* get_sorted_file_list(const char* path) {
    AUXTS__FileList* list = FileList_construct();
    list_files_recursive(list, path);
    FileList_sort(list);

    return list;
}

uint8_t* get_buffer_from_cache_or_sstable(AUXTS__LRUCache* cache, uint64_t stream_id, uint64_t curr_ts, const char* file_path) {
    uint64_t key[2] = {stream_id, curr_ts};
    uint8_t* buffer = AUXTS__LRUCache_get(cache, key);

    if (!buffer) {
        AUXTS__SSTable* sstable = AUXTS__read_sstable_index_entries(file_path);
        buffer = sstable->buffer;
        free(sstable->index_entries);
        free(sstable);

        AUXTS__LRUCache_put(cache, key, buffer);
    }

    return buffer;
}

void process_sstable_buffer(AUXTS__FlacEncodedStream* stream, uint64_t stream_id, uint8_t* buffer) {
    size_t size;
    memcpy(&size, buffer, sizeof(size_t));
    size = AUXTS__swap64_if_big_endian(size);

    AUXTS__SSTable* sstable = AUXTS__read_sstable_index_entries_in_memory(buffer, size);

    for (int j = 0; j < sstable->entry_count; ++j) {
        size_t offset = sstable->index_entries[j].offset;
        AUXTS__MemtableEntry* entry = AUXTS__read_memtable_entry(buffer, offset);

        if (entry->key[0] == stream_id) {
            FlacEncodedStream_append(stream, entry->block, entry->block_size);
        } else {
            free(entry->block);
        }

        free(entry);
    }
    free(sstable);
}

AUXTS__FlacEncodedStream* extract_flac_encoded_blocks(AUXTS__LRUCache* cache, uint64_t stream_id, uint64_t begin_ts, uint64_t end_ts) {
    char* path = get_path_from_time_range(begin_ts, end_ts);

    AUXTS__FileList* list = get_sorted_file_list(path);
    AUXTS__FlacEncodedStream* stream = FlacEncodedStream_construct();

    for (int i = 0; i < list->count; ++i) {
        char* file_path = list->files[i];

        uint64_t curr_ts = time_partitioned_path_to_ts(file_path);
        if (curr_ts >= begin_ts && curr_ts <= end_ts) {
            uint8_t* buffer = get_buffer_from_cache_or_sstable(cache, stream_id, curr_ts, file_path);
            process_sstable_buffer(stream, stream_id, buffer);
        }
    }

    free(path);
    FileList_destroy(list);

    return stream;
}

int test_extract() {
    AUXTS__LRUCache* cache = AUXTS__LRUCache_construct(2);
    AUXTS__FlacEncodedStream* blocks = extract_flac_encoded_blocks(cache, 2426237739028790096, 1739141512213, 1739141512215);

    for (int i = 0; i < blocks->size; ++i) {
        AUXTS__FlacEncodedBlock* block = blocks->blocks[i];
        printf("b %s\n", block->data);
    }

    return 0;
}
