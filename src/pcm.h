
#ifndef RATS_PCM_H
#define RATS_PCM_H

#include "memory.h"
#include "simd/pcm_simd.h"

typedef struct {
    rats_uint8 channels;
    rats_uint8 bit_depth;
    rats_uint32 sample_rate;
    size_t samples;
    rats_memstream out_stream;
} rats_pcm;

void rats_pcm_set_channels(rats_pcm *pcm, rats_uint16 channels);

void rats_pcm_set_bit_depth(rats_pcm *pcm, rats_uint16 bit_depth);

void rats_pcm_set_sample_rate(rats_pcm *pcm, rats_uint32 sample_rate);

void rats_pcm_init(rats_pcm *pcm);

size_t rats_pcm_write_s16(rats_pcm *pcm, const rats_int16 *in, size_t samples);

size_t rats_pcm_write(rats_pcm *pcm, const void *in, size_t samples);

void rats_pcm_destroy(rats_pcm *pcm);

#endif //RATS_PCM_H
