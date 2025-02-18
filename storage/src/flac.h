
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

extern AUXTS_API const int AUXTS__INITIAL_FLAC_STREAM_CAPACITY;

extern AUXTS_API const int AUXTS__INITIAL_PCM_BLOCK_CAPACITY;

typedef struct {
    uint8_t* data;
    uint16_t size;
    uint16_t offset;
} AUXTS__FlacEncodedBlock;

typedef struct {
    AUXTS__FlacEncodedBlock** blocks;
    size_t num_blocks;
    size_t capacity;
} AUXTS__FlacEncodedBlocks;

typedef struct {
    int32_t** data;
    size_t num_samples;
    size_t total_samples;
    uint32_t channels;
    uint32_t sample_rate;
    uint32_t bits_per_sample;
} AUXTS__PcmBlock;

typedef struct {
    AUXTS__FlacEncodedBlock* flac;
    AUXTS__PcmBlock* pcm;
} AUXTS__DecoderContext;

AUXTS_API AUXTS__PcmBlock* AUXTS__decode_flac_block(AUXTS__FlacEncodedBlock* block);
AUXTS_API AUXTS__FlacEncodedBlocks* AUXTS__FlacEncodedBlocks_construct();
AUXTS_API void AUXTS__FlacEncodedBlocks_append(AUXTS__FlacEncodedBlocks* blocks, uint8_t* block_data, uint16_t size);
AUXTS_API void AUXTS__FlacEncodedBlocks_destroy(AUXTS__FlacEncodedBlocks* blocks);

#ifdef __cplusplus
}
#endif

#endif //AUXTS_FLAC_H
