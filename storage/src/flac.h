
#ifndef AUXTS_FLAC_H
#define AUXTS_FLAC_H

#include "export.h"
#include <memory.h>
#include <string.h>
#include <stdlib.h>
#include <FLAC/stream_decoder.h>

typedef struct {
    uint8_t* data;
    uint16_t size;
    uint16_t offset;
} AUXTS__FlacEncodedBlock;

typedef struct {
    AUXTS__FlacEncodedBlock** blocks;
    size_t size;
    size_t capacity;
} AUXTS__FlacEncodedBlocks;

typedef struct {
    int32_t** data;
    size_t size;
    size_t offset;

} AUXTS__PcmBlock;

typedef struct {
    AUXTS__FlacEncodedBlock* flac;
    AUXTS__PcmBlock* pcm;
} AUXTS__DecoderContext;




#endif //AUXTS_FLAC_H
