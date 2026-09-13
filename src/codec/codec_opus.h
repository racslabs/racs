
#ifndef RACS_CODEC_OPUS_H
#define RACS_CODEC_OPUS_H


#ifdef __cplusplus
extern "C" {
#endif


#include "codec_types.h"
#include "../memstream.h"

#include <opusfile.h>
#include <opusenc.h>


int racs_opus_decode(racs_codec_format *fmt,
                     const racs_uint8 *src,
                     size_t src_size,
                     racs_uint8 **out,
                     size_t *out_size);

int racs_opus_encode(racs_codec_format *fmt,
                     const racs_uint8 *src,
                     size_t src_size,
                     racs_uint8 **out,
                     size_t *out_size);


#ifdef __cplusplus
}
#endif


#endif //RACS_CODEC_OPUS_H
