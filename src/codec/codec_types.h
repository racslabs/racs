
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
    RACS_CODEC_ERROR,
    RACS_CODEC_UNSUPPORTED_CHANNELS,
    RACS_CODEC_UNSUPPORTED_SAMPLE_RATE,
    RACS_CODEC_UNSUPPORTED_BITDEPTH
} racs_codec_status;


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
