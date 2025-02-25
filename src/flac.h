
#ifndef AUXTS_FLAC_H
#define AUXTS_FLAC_H

#include "export.h"
#include <memory.h>
#include <string.h>
#include <stdlib.h>
#include <FLAC/stream_decoder.h>

#ifdef __cplusplus
extern "C" {
#endif

#define AUXTS_INITIAL_FLAC_STREAM_CAPACITY 2

typedef struct {
    uint8_t* data;
    uint16_t size;
    uint16_t offset;
} auxts_flac_block;

typedef struct {
    auxts_flac_block** blocks;
    size_t num_blocks;
    size_t capacity;
} auxts_flac_blocks;

typedef struct {
    int32_t** data;
    size_t num_samples;
    size_t total_samples;
    uint32_t channels;
    uint32_t sample_rate;
    uint32_t bits_per_sample;
} auxts_pcm_block;

typedef struct {
    auxts_flac_block* flac;
    auxts_pcm_block* pcm;
} auxts_decoder_context;

auxts_pcm_block* auxts_decode_flac_block(auxts_flac_block* block);
auxts_flac_blocks* auxts_flac_blocks_create();
void auxts_flac_blocks_append(auxts_flac_blocks* blocks, uint8_t* block_data, uint16_t size);
void auxts_flac_blocks_destroy(auxts_flac_blocks* blocks);

#ifdef __cplusplus
}
#endif

#endif //AUXTS_FLAC_H
