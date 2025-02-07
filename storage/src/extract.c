#include "extract.h"

static char* resolve_shared_path(const char* path1, const char* path2);
static int compare_paths(const void* path1, const void* path2);
static AUXTS__FileList* FileList_construct();
static void FileList_add_file(AUXTS__FileList* list, const char* file_path);
static void list_files_recursive(AUXTS__FileList* list, const char* path);
static void FileList_sort(AUXTS__FileList* list);
static uint64_t time_partitioned_path_to_ts(const char* path);
static AUXTS__Block* Block_construct(uint16_t capacity);
static AUXTS__Stream* Stream_construct();
static AUXTS__Stream* extract_stream(AUXTS__LRUCache* cache, uint64_t stream_id, uint64_t begin_ts, uint64_t end_ts);

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

    if (path1[i - 1] == '/') --i;

    char* shared_path = malloc(i + 1);
    strncpy(shared_path, path1, i);
    shared_path[i] = '\0';

    return shared_path;
}

uint64_t time_partitioned_path_to_ts(const char* path) {
    struct tm info;
    struct timespec ts;
    long milliseconds;

    int ret = sscanf(path, ".data/%4d/%2d/%2d/%2d/%2d/%2d/%3ld.df",
                   &info.tm_year,         // Year
                   &info.tm_mon,          // Month (0-based in info structure)
                   &info.tm_mday,         // Day
                   &info.tm_hour,         // Hour
                   &info.tm_min,          // Minute
                   &info.tm_sec,          // Second
                   &milliseconds);

    if (ret < 6) {
        perror("Invalid RFC 3339 format");
        return -1;
    }

    info.tm_year -= 1900;
    info.tm_mon -= 1;

    time_t t = timegm(&info);
    if (t == -1) {
        perror("timegm");
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
            perror("Error reallocating list");
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

void FileList_sort(AUXTS__FileList* list) {
    qsort(list->files, list->count, sizeof(char*), compare_paths);
}

AUXTS__Block* Block_construct(uint16_t capacity) {
    AUXTS__Block* block = malloc(sizeof(AUXTS__Block));
    if (!block) {
        perror("Failed to allocate AUXTS__Block");
        exit(EXIT_FAILURE);
    }

    block->size = 0;
    block->capacity = capacity;

    block->data = malloc(capacity);
    if (!block->data) {
        perror("Failed to allocate data to AUXTS__Block");
        exit(EXIT_FAILURE);
    }

    return block;
}

AUXTS__Stream* extract_stream(AUXTS__LRUCache* cache, uint64_t stream_id, uint64_t begin_ts, uint64_t end_ts) {
    char path1[64], path2[64];

    AUXTS__get_time_partitioned_path(begin_ts, path1);
    AUXTS__get_time_partitioned_path(end_ts, path2);

    char* path = resolve_shared_path(path1, path2);

    AUXTS__FileList* list = FileList_construct();
    list_files_recursive(list, path);
    FileList_sort(list);

    for (int i = 0; i < list->count; ++i) {
        char* file_path = list->files[i];

        uint64_t curr_ts = time_partitioned_path_to_ts(file_path);
        if (curr_ts >= begin_ts || curr_ts <= end_ts) {
            AUXTS__SSTable* sstable;

            uint64_t key[2] = {stream_id, curr_ts};
            uint8_t* data = AUXTS__LRUCache_get(cache, key);

            if (!data) {
                sstable = AUXTS__read_sstable_index_entries(file_path);
                data = sstable->buffer;

                AUXTS__LRUCache_put(cache, key, data);
            }

            sstable = AUXTS__read_sstable_index_entries_in_memory(data, 5);






        }





    }







    free(path);


}

int test_extract() {
    char* path = ".data";
    AUXTS__FileList* list = FileList_construct();
    list_files_recursive(list, path);
    FileList_sort(list);

    for (int i = 0; i < list->count; ++i) {
        printf("%s\n", list->files[i]);
    }

    return 0;
}
