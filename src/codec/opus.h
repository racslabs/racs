
#ifndef RACS_OPUS_H
#define RACS_OPUS_H


#include "../types.h"
#include "../memstream.h"

#include <opusfile.h>
#include <opusenc.h>


typedef struct {
    racs_uint32 sample_rate;
    racs_uint8 channels;
    racs_uint8 bit_depth;
} racs_opus_format;

typedef enum {
    RACS_OPUS_OK,
    RACS_OPUS_DECODE_ERROR,
    RACS_OPUS_ENCODE_ERROR,
    RACS_OPUS_PARAM_ERROR,
    RACS_OPUS_ALLOC_ERROR,
    RACS_OPUS_UNSUPPORTED
} racs_opus_result;


int racs_opus_decode(racs_opus_format *fmt,
                     const racs_uint8 *src,
                     size_t src_size,
                     racs_uint8 **out,
                     size_t *out_size);

int racs_opus_encode(racs_opus_format *fmt,
                     const racs_uint8 *src,
                     size_t src_size,
                     racs_uint8 **out,
                     size_t *out_size);


#endif //RACS_OPUS_H
