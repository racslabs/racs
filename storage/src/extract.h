
#ifndef AUXTS_EXTRACT_H
#define AUXTS_EXTRACT_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>

#define AUXTS__INITIAL_FILE_LIST_CAPACITY 2

#define AUXTS__MAX_PATH_SIZE 255

typedef struct {
    char** files;
    size_t count;
    size_t capacity;
} AUXTS__FileList;

int test_extract();

#endif //AUXTS_EXTRACT_H
