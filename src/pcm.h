
#ifndef RACS_PCM_H
#define RACS_PCM_H

#include "memory.h"
#include "simd/pcm_simd.h"

typedef struct {
    racs_uint8 channels;
    racs_uint8 bit_depth;
    racs_uint32 sample_rate;
    size_t samples;
    racs_memstream out_stream;
} racs_pcm;

void racs_pcm_set_channels(racs_pcm *pcm, racs_uint16 channels);

void racs_pcm_set_bit_depth(racs_pcm *pcm, racs_uint16 bit_depth);

void racs_pcm_set_sample_rate(racs_pcm *pcm, racs_uint32 sample_rate);

void racs_pcm_init(racs_pcm *pcm);

size_t racs_pcm_write_s16(racs_pcm *pcm, const racs_int16 *in, size_t samples);

size_t racs_pcm_write(racs_pcm *pcm, const void *in, size_t samples);

void racs_pcm_destroy(racs_pcm *pcm);

#endif //RACS_PCM_H
