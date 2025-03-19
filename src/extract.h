
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
#include "context.h"
#include "flac.h"

#ifdef __cplusplus
extern "C" {
#endif

extern const char* const auxts_extract_pcm_status_message[];

typedef enum {
    AUXTS_EXTRACT_PCM_STATUS_OK,
    AUXTS_EXTRACT_PCM_STATUS_NOT_FOUND
} auxts_extract_pcm_status;

typedef struct {
    size_t num_samples;
    size_t max_num_samples;
    uint32_t channels;
    uint32_t sample_rate;
    uint32_t bit_depth;
} auxts_pcm_buffer_info;

typedef struct {
    int32_t** data;
    auxts_pcm_buffer_info info;
} auxts_pcm_buffer;

int32_t* auxts_flatten_pcm_data(const auxts_pcm_buffer* pbuf);
void auxts_pcm_buffer_destroy(auxts_pcm_buffer* pbuf);
void auxts_pcm_buffer_init(auxts_pcm_buffer* pbuf, uint32_t channels, uint32_t sample_rate, uint32_t bits_per_sample);
int auxts_extract_pcm_data(auxts_cache* cache, auxts_pcm_buffer* pbuf, uint64_t stream_id, int64_t from, int64_t to);

#ifdef __cplusplus
}
#endif

#endif //AUXTS_EXTRACT_H
