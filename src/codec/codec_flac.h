
#ifndef RACS_CODEC_FLAC_H
#define RACS_CODEC_FLAC_H


#ifdef __cplusplus
extern "C" {
#endif


#include "codec_types.h"
#include "../memstream.h"
#include "../simd/simd.h"

#include <FLAC/stream_decoder.h>
#include <FLAC/stream_encoder.h>


int racs_flac_decode(racs_codec_format *fmt,
                     const racs_uint8 *src,
                     size_t src_size,
                     racs_uint8 **out,
                     size_t *out_size);

int racs_flac_encode(racs_codec_format *fmt,
                     const racs_uint8 *src,
                     size_t src_size,
                     racs_uint8 **out,
                     size_t *out_size);


#ifdef __cplusplus
}
#endif


#endif //RACS_CODEC_FLAC_H
