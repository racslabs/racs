
#ifndef AUXTS_FILELIST_H
#define AUXTS_FILELIST_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>

typedef struct {
    char** files;
    size_t num_files;
    size_t max_num_files;
} auxts_filelist;

void auxts_filelist_add(auxts_filelist* list, const char* file_path);
void auxts_filelist_sort(auxts_filelist* list);
void auxts_filelist_destroy(auxts_filelist* list);
auxts_filelist* auxts_filelist_create();
auxts_filelist* get_sorted_filelist(const char* path);
char* auxts_resolve_shared_path(const char* path1, const char* path2);

#endif //AUXTS_FILELIST_H
