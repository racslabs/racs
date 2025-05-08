#include "filelist.h"

static int path_cmp(const void *path1, const void *path2);

static void list_files_recursive(rats_filelist *list, const char *path);

rats_filelist *rats_filelist_create() {
    rats_filelist *list = malloc(sizeof(rats_filelist));
    if (!list) {
        perror("Failed to allocate rats_filelist");
        rats_filelist_destroy(list);
        return NULL;
    }

    list->num_files = 0;
    list->max_num_files = 2;

    list->files = malloc(2 * sizeof(char *));
    if (!list->files) {
        perror("Failed to allocate file paths to rats_filelist");
        rats_filelist_destroy(list);
        return NULL;
    }

    return list;
}

void rats_filelist_add(rats_filelist *list, const char *file_path) {
    if (list->num_files == list->max_num_files) {
        list->max_num_files *= 2;

        char **files = realloc(list->files, list->max_num_files * sizeof(char *));
        if (!files) {
            perror("Error reallocating file paths to rats_filelist");
            return;
        }

        list->files = files;
    }

    list->files[list->num_files] = strdup(file_path);
    ++list->num_files;
}

void list_files_recursive(rats_filelist *list, const char *path) {
    DIR *dir = opendir(path);
    if (!dir) {
        perror("Failed to open directory");
        closedir(dir);
        return;
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;

        char *file_path;
        asprintf(&file_path, "%s/%s", path, entry->d_name);

        if (entry->d_type == DT_DIR) {
            list_files_recursive(list, file_path);
        } else if (entry->d_type == DT_REG) {
            rats_filelist_add(list, file_path);
        }

        free(file_path);
    }

    closedir(dir);
}

void rats_filelist_destroy(rats_filelist *list) {
    for (int i = 0; i < list->num_files; ++i) {
        free(list->files[i]);
    }

    free(list->files);
    free(list);
}

rats_filelist *get_sorted_filelist(const char *path) {
    rats_filelist *list = rats_filelist_create();
    list_files_recursive(list, path);
    rats_filelist_sort(list);

    return list;
}

char *rats_resolve_shared_path(const char *path1, const char *path2) {
    if (!path1 || !path2) {
        perror("Paths cannot be null");
        return NULL;
    }

    size_t len1 = strlen(path1);
    size_t len2 = strlen(path2);
    size_t len = len1 < len2 ? len1 : len2;

    size_t i = 0;
    for (; i < len; i++)
        if (path1[i] != path2[i]) break;

    while (i > 1 && path1[i - 1] != '/') --i;
    --i;

    char *shared_path = malloc(i + 1);
    strncpy(shared_path, path1, i);
    shared_path[i] = '\0';

    return shared_path;
}

void rats_filelist_sort(rats_filelist *list) {
    qsort(list->files, list->num_files, sizeof(char *), path_cmp);
}

int path_cmp(const void *path1, const void *path2) {
    return strcmp(*(const char **) path1, *(const char **) path2);
}
