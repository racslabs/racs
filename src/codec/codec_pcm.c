
#include "codec_pcm.h"


int racs_pcm_decode(racs_codec_format *fmt,
                    const racs_uint8 *src,
                    size_t src_size,
                    racs_uint8 **out,
                    size_t *out_size) {
    if (!fmt || !src || src_size == 0 || !out || !out_size) {
        return RACS_CODEC_ERROR;
    }

    *out = NULL;
    *out_size = 0;

    racs_uint8 *buf = malloc(src_size);
    if (!buf) {
        return RACS_CODEC_ERROR;
    }

    memcpy(buf, src, src_size);

    *out = buf;
    *out_size = src_size;

    return RACS_CODEC_OK;
}

int racs_pcm_encode(racs_codec_format *fmt,
                    const racs_uint8 *src,
                    size_t src_size,
                    racs_uint8 **out,
                    size_t *out_size) {
    if (!fmt || !src || src_size == 0 || !out || !out_size) {
        return RACS_CODEC_ERROR;
    }

    *out = NULL;
    *out_size = 0;

    racs_uint8 *buf = malloc(src_size);
    if (!buf) {
        return RACS_CODEC_ERROR;
    }

    memcpy(buf, src, src_size);

    *out = buf;
    *out_size = src_size;

    return RACS_CODEC_OK;
}
