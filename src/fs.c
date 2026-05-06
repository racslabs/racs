#include "fs.h"


void racs_fs_mkdir(const char *path) {
    char dir[PATH_MAX];
    snprintf(dir, sizeof(dir), "%s", path);

    char *p = dir;
    while ((p = strchr(p + 1, '/')) != NULL) {
        *p = '\0';
        if (mkdir(dir, 0777) == -1 && errno != EEXIST) {
            *p = '/';
            return;
        }
        *p = '/';
    }
}

void racs_fs_walk(const char *path, racs_fs_walk_callback callback, void *data) {
    DIR *dir = opendir(path);
    if (!dir) {
        return;
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        char full_path[PATH_MAX];
        snprintf(full_path, sizeof(full_path), "%s/%s", path, entry->d_name);

        if (entry->d_type == DT_DIR) {
            racs_fs_walk(full_path, callback, data);
        } else if (entry->d_type == DT_REG) {
            callback(full_path, data);
        }
    }

    closedir(dir);
}