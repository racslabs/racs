#ifndef RACS_CODEC_MP3_H
#define RACS_CODEC_MP3_H


#ifdef __cplusplus
extern "C" {
#endif


#include "codec_types.h"
#include "../memstream.h"
#include <mpg123.h>
#include <lame/lame.h>


int racs_mp3_decode(racs_codec_format *fmt,
                    const racs_uint8 *src,
                    size_t src_size,
                    racs_uint8 **out,
                    size_t *out_size);


int racs_mp3_encode(racs_codec_format *fmt,
                    const racs_uint8 *src,
                    size_t src_size,
                    racs_uint8 **out,
                    size_t *out_size);


#ifdef __cplusplus
}
#endif

#endif //RACS_CODEC_MP3_H
