
#ifndef RACS_FLAC_H
#define RACS_FLAC_H


#ifdef __cplusplus
extern "C" {
#endif


#include "../types.h"
#include "../memstream.h"
#include "../simd/simd.h"

#include <FLAC/stream_decoder.h>
#include <FLAC/stream_encoder.h>


typedef struct {
    racs_uint32 sample_rate;
    racs_uint8 channels;
    racs_uint8 bit_depth;
} racs_flac_format;

typedef enum {
    RACS_FLAC_OK,
    RACS_FLAC_DECODE_ERROR,
    RACS_FLAC_ENCODE_ERROR,
    RACS_FLAC_PARAM_ERROR,
    RACS_FLAC_ALLOC_ERROR,
    RACS_FLAC_UNSUPPORTED
} racs_flac_result;


int racs_flac_decode(racs_flac_format *fmt,
                     const racs_uint8 *src,
                     size_t src_size,
                     racs_uint8 **out,
                     size_t *out_size);

int racs_flac_encode(racs_flac_format *fmt,
                     const racs_uint8 *src,
                     size_t src_size,
                     racs_uint8 **out,
                     size_t *out_size);


#ifdef __cplusplus
}
#endif


#endif //RACS_FLAC_H
