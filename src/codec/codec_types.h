
#ifndef RACS_CODEC_TYPES_H
#define RACS_CODEC_TYPES_H


#include "../types.h"


typedef struct {
    racs_uint32 sample_rate;
    racs_uint8 channels;
    racs_uint8 bit_depth;
} racs_codec_format;


typedef enum {
    RACS_CODEC_OK,
    RACS_CODEC_DECODE_ERROR,
    RACS_CODEC_ENCODE_ERROR,
    RACS_CODEC_PARAM_ERROR,
    RACS_CODEC_ALLOC_ERROR,
    RACS_CODEC_UNSUPPORTED
} racs_codec_result;


typedef enum {
    RACS_CODEC_UNKNOWN,
    RACS_CODEC_PCM,
    RACS_CODEC_MP3,
    RACS_CODEC_AAC,
    RACS_CODEC_WAV,
    RACS_CODEC_OPUS,
    RACS_CODEC_FLAC
} racs_codec;


#endif //RACS_CODEC_TYPES_H
