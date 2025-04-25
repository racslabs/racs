
#ifndef RATS_FILELIST_H
#define RATS_FILELIST_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>

typedef struct {
    char** files;
    size_t num_files;
    size_t max_num_files;
} rats_filelist;

void rats_filelist_add(rats_filelist* list, const char* file_path);
void rats_filelist_sort(rats_filelist* list);
void rats_filelist_destroy(rats_filelist* list);
rats_filelist* rats_filelist_create();
rats_filelist* get_sorted_filelist(const char* path);
char* rats_resolve_shared_path(const char* path1, const char* path2);

#endif //RATS_FILELIST_H
