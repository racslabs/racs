
#ifndef AUXTS_PCM_H
#define AUXTS_PCM_H

#include "stream.h"
#include "simd/pcm_simd.h"

typedef enum {
    AUXTS_PCM_16 = 16,
    AUXTS_PCM_24 = 24
} auxts_pcm_bit_depth;

typedef struct {
    auxts_uint8  channels;
    auxts_uint8  bit_depth;
    auxts_uint32 sample_rate;
    size_t       samples;
    auxts_memory_stream out_stream;
} auxts_pcm;

void auxts_pcm_set_channels(auxts_pcm* pcm, auxts_uint16 channels);
void auxts_pcm_set_bit_depth(auxts_pcm* pcm, auxts_uint16 bit_depth);
void auxts_pcm_set_sample_rate(auxts_pcm* pcm, auxts_uint32 sample_rate);

void auxts_pcm_init(auxts_pcm* pcm, void* out, size_t size);

size_t auxts_pcm_write_s16(auxts_pcm* pcm, const auxts_int16* in, size_t samples);
size_t auxts_pcm_write(auxts_pcm* pcm, const void* in, size_t samples);

#endif //AUXTS_PCM_H
