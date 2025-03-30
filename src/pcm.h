
#ifndef AUXTS_PCM_H
#define AUXTS_PCM_H

#include "stream.h"

typedef struct {
    auxts_uint8  channels;
    auxts_uint8  bit_depth;
    auxts_uint32 sample_rate;
    size_t       sample_count;
    auxts_memory_stream memory_stream;
} auxts_pcm;

void auxts_pcm_init_s32(auxts_pcm* pcm, auxts_int32* buf, auxts_uint64 n);
void auxts_pcm_init_s16(auxts_pcm* pcm, auxts_int16* buf, auxts_uint64 n);

void auxts_pcm_write_s32(auxts_pcm* pcm, auxts_int32* samples, auxts_uint64 n);
void auxts_pcm_write_s16(auxts_pcm* pcm, auxts_int16* samples, auxts_uint64 n);

#endif //AUXTS_PCM_H
