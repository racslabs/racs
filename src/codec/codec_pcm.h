#ifndef RACS_CODEC_PCM_H
#define RACS_CODEC_PCM_H


#include "codec_types.h"
#include <string.h>


int racs_pcm_decode(racs_codec_format *fmt,
                    const racs_uint8 *src,
                    size_t src_size,
                    racs_uint8 **out,
                    size_t *out_size);

int racs_pcm_encode(racs_codec_format *fmt,
                    const racs_uint8 *src,
                    size_t src_size,
                    racs_uint8 **out,
                    size_t *out_size);
                    
#endif //RACS_CODEC_PCM