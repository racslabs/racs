
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
#include "auxts.h"
#include "flac.h"

#ifdef __cplusplus
extern "C" {
#endif

#define AUXTS_INITIAL_PCM_BUFFER_CAPACITY 2

typedef enum {
    AUXTS_EXTRACT_PCM_STATUS_OK,
    AUXTS_EXTRACT_PCM_STATUS_NO_DATA,
    AUXTS_EXTRACT_PCM_STATUS_INVALID_TIMESTAMP
} auxts_extract_pcm_status;

extern const char* const auxts_extract_pcm_status_message[];
extern const char* const auxts_extract_pcm_status_code[];

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

uint8_t* auxts_pack_pcm_data(const auxts_pcm_buffer* pbuf);
void auxts_pcm_buffer_destroy(auxts_pcm_buffer* pbuf);
void auxts_pcm_buffer_init(auxts_pcm_buffer* pbuf, uint32_t channels, uint32_t sample_rate, uint32_t bits_per_sample);
auxts_extract_pcm_status auxts_extract_pcm_data(auxts_cache* cache, auxts_pcm_buffer* pbuf, uint64_t stream_id, int64_t from, int64_t to);

#ifdef __cplusplus
}
#endif

#endif //AUXTS_EXTRACT_H
