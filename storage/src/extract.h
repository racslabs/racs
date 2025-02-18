
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

#ifdef __cplusplus
extern "C" {
#endif

extern AUXTS_API const int AUXTS__INITIAL_FILE_LIST_CAPACITY;

extern AUXTS_API const int AUXTS__INITIAL_PCM_BUFFER_CAPACITY;

typedef struct {
    char** files;
    size_t num_files;
    size_t max_num_files;
} AUXTS__FileList;

typedef struct {
    int32_t** data;
    size_t num_samples;
    size_t max_num_samples;
    uint32_t channels;
    uint32_t sample_rate;
    uint32_t bits_per_sample;
} AUXTS__PcmBuffer;

AUXTS_API AUXTS__PcmBuffer* AUXTS__extract_pcm_data(AUXTS__LRUCache* cache, uint64_t stream_id, uint64_t begin_timestamp, uint64_t end_timestamp);
AUXTS_API void AUXTS__PcmBuffer_destroy(AUXTS__PcmBuffer* buffer);

int test_extract();

#ifdef __cplusplus
}
#endif

#endif //AUXTS_EXTRACT_H
