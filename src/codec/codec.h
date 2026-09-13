
#ifndef RACS_CODEC_H
#define RACS_CODEC_H


#include "codec_pcm.h"
#include "codec_aac.h"
#include "codec_mp3.h"
#include "codec_opus.h"
#include "codec_flac.h"
#include "../racs_string.h"


int racs_codec_decode(racs_codec codec,
                      racs_codec_format *fmt,
                      const racs_uint8 *src,
                      size_t src_size,
                      racs_uint8 **out,
                      size_t *out_size);

int racs_codec_encode(racs_codec codec,
                      racs_codec_format *fmt,
                      const racs_uint8 *src,
                      size_t src_size,
                      racs_uint8 **out,
                      size_t *out_size);


int racs_codec_from_string(const char *codec);


#endif //RACS_CODEC_H
