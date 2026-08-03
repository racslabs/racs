
#ifndef RACS_AAC_H
#define RACS_AAC_H


#ifdef __cplusplus
extern "C" {
#endif


#include "types.h"
#include "memstream.h"
#include <fdk-aac/aacenc_lib.h>
#include <fdk-aac/aacdecoder_lib.h>


#define RACS_AAC_BIT_DEPTH 16


typedef struct {
    racs_uint32 sample_rate;
    racs_uint8 channels;
    racs_uint8 bit_depth;
} racs_aac_format;

typedef enum {
    RACS_AAC_OK,
    RACS_AAC_DECODE_ERROR,
    RACS_AAC_ENCODE_ERROR,
    RACS_AAC_PARAM_ERROR,
    RACS_AAC_ALLOC_ERROR,
    RACS_AAC_UNSUPPORTED
} racs_aac_result;


int racs_aac_decode(racs_aac_format *fmt,
                    const racs_uint8 *src,
                    size_t src_size,
                    racs_uint8 **out,
                    size_t *out_size);

int racs_aac_encode(racs_aac_format *fmt,
                    const racs_uint8 *src,
                    size_t src_size,
                    racs_uint8 **out,
                    size_t *out_size);


#ifdef __cplusplus
}
#endif

#endif //RACS_AAC_H
