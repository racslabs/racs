
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
} FlacEncodedBlock;

typedef struct {
    FlacEncodedBlock** blocks;
    size_t num_blocks;
    size_t capacity;
} FlacEncodedBlocks;

typedef struct {
    int32_t** data;
    size_t num_samples;
    size_t total_samples;
    uint32_t channels;
    uint32_t sample_rate;
    uint32_t bits_per_sample;
} PcmBlock;

typedef struct {
    FlacEncodedBlock* flac;
    PcmBlock* pcm;
} DecoderContext;

PcmBlock* auxts_decode_flac_block(FlacEncodedBlock* block);
FlacEncodedBlocks* auxts_flac_encoded_blocks_create();
void auxts_flac_encoded_blocks_append(FlacEncodedBlocks* blocks, uint8_t* block_data, uint16_t size);
void auxts_flac_encoded_blocks_destroy(FlacEncodedBlocks* blocks);

#ifdef __cplusplus
}
#endif

#endif //AUXTS_FLAC_H
