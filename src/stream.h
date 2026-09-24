
#ifndef RACS_STREAM_H
#define RACS_STREAM_H

#ifdef __cplusplus
extern "C" {
#endif


#include "mt.h"
#include "codec/codec.h"
#include "mmh3.h"
#include "info.h"
#include "offsets.h"
#include "crc32c.h"
#include "racs_zstd.h"
#include "racs_string.h"


typedef enum {
    RACS_STREAM_OK = 0,
    RACS_STREAM_NOT_FOUND,
    RACS_STREAM_CONFLICT,
    RACS_STREAM_DECODE_ERROR,
    RACS_STREAM_INTERNAL_ERROR,  
    RACS_STREAM_UNKNOWN_CODEC,
    RACS_STREAM_INVALID_BITDEPTH,
    RACS_STREAM_INVALID_SAMPLE_RATE,
    RACS_STREAM_INVALID_CHANNELS,
    RACS_STREAM_INGESTION_ERROR
} racs_stream_result;


extern const char *const racs_stream_result_string[];


int racs_streams_create(const char *stream_id,
                        racs_uint32 sample_rate,
                        racs_uint8 channels,
                        racs_uint8 bit_depth);

int racs_stream(const char *stream_id, 
                const char *s_codec, 
                const racs_uint8 *src,
                size_t src_size);                        


#ifdef __cplusplus
}
#endif

#endif //RACS_STREAM_H
