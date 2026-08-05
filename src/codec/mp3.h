#ifndef RACS_MP3_H
#define RACS_MP3_H


#ifdef __cplusplus
extern "C" {
#endif


#include "../types.h"
#include "../memstream.h"
#include <mpg123.h>
#include <lame/lame.h>


typedef struct {
    racs_uint32 sample_rate;
    racs_uint8 channels;
    racs_uint8 bit_depth;
} racs_mp3_format;

typedef enum {
    RACS_MP3_OK,
    RACS_MP3_DECODE_ERROR,
    RACS_MP3_ENCODE_ERROR,
    RACS_MP3_PARAM_ERROR,
    RACS_MP3_ALLOC_ERROR,
    RACS_MP3_UNSUPPORTED
} racs_mp3_result;


int racs_mp3_decode(racs_mp3_format *fmt,
                    const racs_uint8 *src,
                    size_t src_size,
                    racs_uint8 **out,
                    size_t *out_size);


int racs_mp3_encode(racs_mp3_format *fmt,
                    const racs_uint8 *src,
                    size_t src_size,
                    racs_uint8 **out,
                    size_t *out_size);


#ifdef __cplusplus
}
#endif

#endif //RACS_MP3_H
