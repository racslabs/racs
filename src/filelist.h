
#ifndef RACS_FILELIST_H
#define RACS_FILELIST_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>

typedef struct {
    char **files;
    size_t num_files;
    size_t max_num_files;
} racs_filelist;

void racs_filelist_add(racs_filelist *list, const char *file_path);

void racs_filelist_sort(racs_filelist *list);

void racs_filelist_destroy(racs_filelist *list);

racs_filelist *racs_filelist_create();

racs_filelist *get_sorted_filelist(const char *path);

char *racs_resolve_shared_path(const char *path1, const char *path2);

#endif //RACS_FILELIST_H
