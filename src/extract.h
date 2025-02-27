
#ifndef AUXTS_EXTRACT_H
#define AUXTS_EXTRACT_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <msgpack.h>
#include "memtable.h"
#include "filelist.h"
#include "result.h"
#include "cache.h"
#include "flac.h"

#ifdef __cplusplus
extern "C" {
#endif

#define AUXTS_INITIAL_PCM_BUFFER_CAPACITY 2

typedef struct {
    size_t num_samples;
    size_t max_num_samples;
    uint32_t channels;
    uint32_t sample_rate;
    uint32_t bits_per_sample;
} auxts_pcm_buffer_info;

typedef struct {
    int32_t** data;
    auxts_pcm_buffer_info info;
} auxts_pcm_buffer;

auxts_result auxts_extract(auxts_cache* cache, uint64_t stream_id, uint64_t begin_timestamp, uint64_t end_timestamp);

#ifdef __cplusplus
}
#endif

#endif //AUXTS_EXTRACT_H
