
#ifndef RACS_CODEC_AAC_H
#define RACS_CODEC_AAC_H


#ifdef __cplusplus
extern "C" {
#endif


#include "codec_types.h"
#include "../memstream.h"
#include <fdk-aac/aacenc_lib.h>
#include <fdk-aac/aacdecoder_lib.h>


int racs_aac_decode(racs_codec_format *fmt,
                    const racs_uint8 *src,
                    size_t src_size,
                    racs_uint8 **out,
                    size_t *out_size);

int racs_aac_encode(racs_codec_format *fmt,
                    const racs_uint8 *src,
                    size_t src_size,
                    racs_uint8 **out,
                    size_t *out_size);


#ifdef __cplusplus
}
#endif


#endif //RACS_CODEC_AAC_H
