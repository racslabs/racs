
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
#include "flac.h"

#define AUXTS__INITIAL_FILE_LIST_CAPACITY 2

#define AUXTS__INITIAL_BLOCK_STREAM_CAPACITY 2

#define AUXTS__MAX_PATH_SIZE 255

typedef struct {
    char** files;
    size_t count;
    size_t capacity;
} AUXTS__FileList;

typedef struct {
    int32_t** data;
    size_t size;
    size_t offset;
    uint32_t channels;
    uint32_t sample_rate;
    uint32_t bits_per_sample;
} AUXTS__PcmBuffer;

int test_extract();

#endif //AUXTS_EXTRACT_H
