
#include "codec.h"


int racs_codec_decode(racs_codec codec,
                      racs_codec_format *fmt,
                      const racs_uint8 *src,
                      size_t src_size,
                      racs_uint8 **out,
                      size_t *out_size) {
    switch (codec) {
        case RACS_CODEC_PCM:
            return racs_pcm_decode(fmt, src, src_size, out, out_size);
        case RACS_CODEC_AAC:
            return racs_aac_decode(fmt, src, src_size, out, out_size);
        case RACS_CODEC_MP3:
            return racs_mp3_decode(fmt, src, src_size, out, out_size);
        case RACS_CODEC_FLAC:
            return racs_flac_decode(fmt, src, src_size, out, out_size);
        case RACS_CODEC_OPUS:
            return racs_opus_decode(fmt, src, src_size, out, out_size);
        default:
            return RACS_CODEC_UNKNOWN;
    }                        
}

int racs_codec_encode(racs_codec codec,
                      racs_codec_format *fmt,
                      const racs_uint8 *src,
                      size_t src_size,
                      racs_uint8 **out,
                      size_t *out_size) {
    switch (codec) {
        case RACS_CODEC_PCM:
            return racs_pcm_encode(fmt, src, src_size, out, out_size);
        case RACS_CODEC_AAC:
            return racs_aac_encode(fmt, src, src_size, out, out_size);
        case RACS_CODEC_MP3:
            return racs_mp3_encode(fmt, src, src_size, out, out_size);
        case RACS_CODEC_FLAC:
            return racs_flac_encode(fmt, src, src_size, out, out_size);
        case RACS_CODEC_OPUS:
            return racs_opus_encode(fmt, src, src_size, out, out_size);
        default:
            return RACS_CODEC_UNKNOWN;
    }
}

int racs_codec_from_string(const char *codec) {
    char *codec_trim = strdup(codec);
    racs_trim(codec_trim);

    int status = RACS_CODEC_UNKNOWN;

    if (strcasecmp(codec_trim, "pcm") == 0) {
        status = RACS_CODEC_PCM;
    }

    if (strcasecmp(codec_trim, "mp3") == 0) {
        status = RACS_CODEC_MP3;
    }

    if (strcasecmp(codec_trim, "aac") == 0) {
        status = RACS_CODEC_AAC;
    }

    if (strcasecmp(codec_trim, "flac") == 0) {
        status = RACS_CODEC_FLAC;
    }

    if (strcasecmp(codec_trim, "opus") == 0) {
        status = RACS_CODEC_OPUS;
    }

    free(codec_trim);
    return status;
}
