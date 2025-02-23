
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

#define AUXTS_INITIAL_FILE_LIST_CAPACITY 2

#define AUXTS_INITIAL_PCM_BUFFER_CAPACITY 2

typedef struct {
    char** files;
    size_t num_files;
    size_t max_num_files;
} file_list;

typedef struct {
    int32_t** data;
    size_t num_samples;
    size_t max_num_samples;
    uint32_t channels;
    uint32_t sample_rate;
    uint32_t bits_per_sample;
} pcm_buffer;

pcm_buffer* auxts_extract_pcm_data(lru_cache* cache, uint64_t stream_id, uint64_t begin_timestamp, uint64_t end_timestamp);
void auxts_pcm_buffer_destroy(pcm_buffer* buffer);

#ifdef __cplusplus
}
#endif

#endif //AUXTS_EXTRACT_H
