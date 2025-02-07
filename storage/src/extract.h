
#ifndef AUXTS_EXTRACT_H
#define AUXTS_EXTRACT_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include "memtable.h"
#include "cache.h"

#define AUXTS__INITIAL_FILE_LIST_CAPACITY 2

#define AUXTS__INITIAL_STREAM_CAPACITY 2

#define AUXTS__MAX_PATH_SIZE 255

typedef struct {
    char** files;
    size_t count;
    size_t capacity;
} AUXTS__FileList;

typedef struct {
    uint8_t* data;
    uint16_t size;
    uint16_t capacity;
} AUXTS__Block;

typedef struct {
    AUXTS__Block** blocks;
    size_t size;
    size_t capacity;
} AUXTS__Stream;

int test_extract();

#endif //AUXTS_EXTRACT_H
