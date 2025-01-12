#include "extract.h"

static char* resolve_shared_path(const char* path1, const char* path2);
static int compare_paths(const void* path1, const void* path2);
static AUXTS__FileList* FileList_construct();
static void FileList_add_file(AUXTS__FileList* list, const char* file_path);
static void list_files_recursive(AUXTS__FileList* list, const char* path);
static void FileList_sort(AUXTS__FileList* list);

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
