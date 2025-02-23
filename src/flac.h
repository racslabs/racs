
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
} flac_encoded_block;

typedef struct {
    flac_encoded_block** blocks;
    size_t num_blocks;
    size_t capacity;
} flac_encoded_blocks;

typedef struct {
    int32_t** data;
    size_t num_samples;
    size_t total_samples;
    uint32_t channels;
    uint32_t sample_rate;
    uint32_t bits_per_sample;
} pcm_block;

typedef struct {
    flac_encoded_block* flac;
    pcm_block* pcm;
} decoder_context;

pcm_block* auxts_decode_flac_block(flac_encoded_block* block);
flac_encoded_blocks* auxts_flac_encoded_blocks_create();
void auxts_flac_encoded_blocks_append(flac_encoded_blocks* blocks, uint8_t* block_data, uint16_t size);
void auxts_flac_encoded_blocks_destroy(flac_encoded_blocks* blocks);

#ifdef __cplusplus
}
#endif

#endif //AUXTS_FLAC_H
