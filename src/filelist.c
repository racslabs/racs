#include "filelist.h"

static int path_cmp(const void* path1, const void* path2);
static void list_files_recursive(auxts_filelist* list, const char* path);

auxts_filelist* auxts_filelist_create() {
    auxts_filelist* list = malloc(sizeof(auxts_filelist));
    if (!list) {
        perror("Failed to allocate auxts_filelist");
        auxts_filelist_destroy(list);
        return NULL;
    }

    list->num_files = 0;
    list->max_num_files = 2;

    list->files = malloc(2 * sizeof(char*));
    if (!list->files) {
        perror("Failed to allocate file paths to auxts_filelist");
        auxts_filelist_destroy(list);
        return NULL;
    }

    return list;
}

void auxts_filelist_add(auxts_filelist* list, const char* file_path) {
    if (list->num_files == list->max_num_files) {
        list->max_num_files *= 2;

        char** files = realloc(list->files, list->max_num_files * sizeof(char*));
        if (!files) {
            perror("Error reallocating file paths to auxts_filelist");
            return;
        }

        list->files = files;
    }

    list->files[list->num_files] = strdup(file_path);
    ++list->num_files;
}

void list_files_recursive(auxts_filelist* list, const char* path) {
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
            auxts_filelist_add(list, file_path);
        }
    }

    closedir(dir);
}

void auxts_filelist_destroy(auxts_filelist* list) {
    for (int i = 0; i < list->num_files; ++i) {
        free(list->files[i]);
    }

    free(list->files);
    free(list);
}

auxts_filelist* get_sorted_filelist(const char* path) {
    auxts_filelist* list = auxts_filelist_create();
    list_files_recursive(list, path);
    auxts_filelist_sort(list);

    return list;
}

char* auxts_resolve_shared_path(const char* path1, const char* path2) {
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

void auxts_filelist_sort(auxts_filelist* list) {
    qsort(list->files, list->num_files, sizeof(char*), path_cmp);
}

int path_cmp(const void* path1, const void* path2) {
    return strcmp(*(const char**)path1, *(const char**) path2);
}
