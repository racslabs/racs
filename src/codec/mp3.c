#include "mp3.h"


typedef struct {
    mpg123_handle *mh;
    racs_uint8 *pcm_block;
    size_t pcm_block_size;
} racs_mp3_decoder;


typedef struct {
    lame_global_flags *gfp;
} racs_mp3_encoder;


int racs_mp3_decoder_init(racs_mp3_decoder *dec);

int racs_mp3_decoder_decode(racs_mp3_decoder *dec,
                            racs_mp3_format *fmt,
                            const racs_uint8 *src,
                            size_t src_size,
                            racs_memstream *ms);

void racs_mp3_decoder_cleanup(racs_mp3_decoder *dec);

int racs_mp3_encoder_init(racs_mp3_encoder *enc, racs_mp3_format *fmt);

int racs_mp3_encoder_encode(racs_mp3_encoder *enc,
                            racs_mp3_format *fmt,
                            const racs_uint8 *src,
                            size_t src_size,
                            racs_memstream *ms);

void racs_mp3_encoder_cleanup(racs_mp3_encoder *enc);


int racs_mp3_decoder_init(racs_mp3_decoder *dec) {
    int err = MPG123_OK;

    dec->mh = mpg123_new(NULL, &err);
    if (!dec->mh || err != MPG123_OK) {
        return RACS_MP3_ALLOC_ERROR;
    }

    if (mpg123_open_feed(dec->mh) != MPG123_OK) {
        mpg123_delete(dec->mh);
        return RACS_MP3_ALLOC_ERROR;
    }

    dec->pcm_block_size = mpg123_outblock(dec->mh);
    dec->pcm_block = malloc(dec->pcm_block_size);
    if (!dec->pcm_block) {
        mpg123_delete(dec->mh);
        return RACS_MP3_ALLOC_ERROR;
    }

    return RACS_MP3_OK;
}

int racs_mp3_decoder_decode(racs_mp3_decoder *dec,
                            racs_mp3_format *fmt,
                            const racs_uint8 *src,
                            size_t src_size,
                            racs_memstream *ms) {
    size_t bytes_read = 0;
    size_t bytes_decoded = 0;

    int status = MPG123_NEED_MORE;
    int fmt_ext = 0;

    while (1) {
        if (status == MPG123_NEED_MORE && bytes_read < src_size) {
            size_t chunk = (src_size - bytes_read > 4096) ? 4096 : (src_size - bytes_read);
            mpg123_feed(dec->mh, src + bytes_read, chunk);
            bytes_read += chunk;
        }

        status = mpg123_read(dec->mh, dec->pcm_block, dec->pcm_block_size, &bytes_decoded);

        if (bytes_decoded > 0) {
            racs_memstream_write(ms, dec->pcm_block, bytes_decoded);

            if (!fmt_ext) {
                long sample_rate = 0;
                int channels = 0;
                int encoding = 0;

                if (mpg123_getformat(dec->mh, &sample_rate, &channels, &encoding) == MPG123_OK) {
                    fmt->sample_rate = (racs_uint32) sample_rate;
                    fmt->channels = (racs_uint8) channels;
                    fmt->bit_depth = (racs_uint8) (mpg123_encsize(encoding) * 8);

                    fmt_ext = 1;
                }
            }
        }

        if (status == MPG123_DONE) {
            return RACS_MP3_OK;
        }

        if (status == MPG123_ERR) {
            return RACS_MP3_DECODE_ERROR;
        }

        if (status == MPG123_NEED_MORE && bytes_read >= src_size) {
            return RACS_MP3_OK;
        }
    }
}

void racs_mp3_decoder_cleanup(racs_mp3_decoder *dec) {
    if (!dec) {
        return;
    }

    if (dec->pcm_block) {
        free(dec->pcm_block);
    }

    if (dec->mh) {
        mpg123_close(dec->mh);
        mpg123_delete(dec->mh);
    }
}

int racs_mp3_decode(racs_mp3_format *fmt,
                    const racs_uint8 *src,
                    size_t src_size,
                    racs_uint8 **out,
                    size_t *out_size) {
    if (!src || src_size == 0 || !out || !out_size || !fmt) {
        return RACS_MP3_PARAM_ERROR;
    }

    *out = NULL;
    *out_size = 0;

    size_t buf_size = 0;
    racs_uint8 *buf = malloc(1024);
    if (!buf) {
        return RACS_MP3_ALLOC_ERROR;
    }

    racs_memstream ms = {
        .data = &buf,
        .size = &buf_size,
        .capacity = 1024,
    };

    racs_mp3_decoder dec;
    int status = racs_mp3_decoder_init(&dec);
    if (status != RACS_MP3_OK) {
        free(buf);
        return status;
    }

    status = racs_mp3_decoder_decode(&dec, fmt, src, src_size, &ms);
    if (status != RACS_MP3_OK) {
        free(buf);
        racs_mp3_decoder_cleanup(&dec);
        return status;
    }

    *out = buf;
    *out_size = buf_size;

    racs_mp3_decoder_cleanup(&dec);
    return status;
}

int racs_mp3_encoder_init(racs_mp3_encoder *enc, racs_mp3_format *fmt) {
    enc->gfp = lame_init();
    if (!enc->gfp) {
        return RACS_MP3_ALLOC_ERROR;
    }

    lame_set_num_channels(enc->gfp, fmt->channels);
    lame_set_in_samplerate(enc->gfp, (int) fmt->sample_rate);
    lame_set_VBR(enc->gfp, vbr_default);
    lame_set_quality(enc->gfp, 2);

    if (lame_init_params(enc->gfp) < 0) {
        lame_close(enc->gfp);
        return RACS_MP3_ALLOC_ERROR;
    }

    return RACS_MP3_OK;
}

int racs_mp3_encoder_encode(racs_mp3_encoder *enc,
                            racs_mp3_format *fmt,
                            const racs_uint8 *src,
                            size_t src_size,
                            racs_memstream *ms) {
    if (fmt->bit_depth != 16) {
        return RACS_MP3_UNSUPPORTED;
    }

    int frames_per_chunk = 4096;
    int buf_size = 1.25 * frames_per_chunk + 7200;

    racs_uint8 *buf = malloc(buf_size);
    if (!buf) {
        return RACS_MP3_ALLOC_ERROR;
    }

    int bytes_encoded = 0;

    size_t samples_read = 0;
    size_t total_samples = src_size / sizeof(racs_uint16);

    racs_int16 *samples = (racs_int16 *) src;

    while (samples_read < total_samples) {
        size_t remaining = (total_samples - samples_read) / fmt->channels;
        size_t chunk_size = (remaining > frames_per_chunk) ? frames_per_chunk : remaining;

        switch (fmt->channels) {
            case 1:
                bytes_encoded = lame_encode_buffer(enc->gfp, samples + samples_read,
                                                   NULL, (int) chunk_size,
                                                   buf, buf_size);
                break;
            case 2:
                bytes_encoded = lame_encode_buffer_interleaved(enc->gfp, samples + samples_read,
                                                               (int) chunk_size, buf,
                                                               buf_size);
                break;
            default:
                free(buf);
                return RACS_MP3_ENCODE_ERROR;
        }

        if (bytes_encoded > 0) {
            racs_memstream_write(ms, buf, bytes_encoded);
        }

        samples_read += (chunk_size * fmt->channels);
    }

    bytes_encoded = lame_encode_flush(enc->gfp, buf, buf_size);
    if (bytes_encoded > 0) {
        racs_memstream_write(ms, buf, bytes_encoded);
    }

    free(buf);
    return RACS_MP3_OK;
}

void racs_mp3_encoder_cleanup(racs_mp3_encoder *enc) {
    if (enc->gfp) {
        lame_close(enc->gfp);
    }
}

int racs_mp3_encode(racs_mp3_format *fmt,
                    const racs_uint8 *src,
                    size_t src_size,
                    racs_uint8 **out,
                    size_t *out_size) {
    if (!src || src_size == 0 || !out || !out_size || !fmt) {
        return RACS_MP3_PARAM_ERROR;
    }

    *out = NULL;
    *out_size = 0;

    size_t buf_size = 0;
    racs_uint8 *buf = malloc(1024);
    if (!buf) {
        return RACS_MP3_ALLOC_ERROR;
    }

    racs_memstream ms = {
        .data = &buf,
        .size = &buf_size,
        .capacity = 1024,
    };

    racs_mp3_encoder enc;
    int status = racs_mp3_encoder_init(&enc, fmt);
    if (status != RACS_MP3_OK) {
        free(buf);
        return status;
    }

    status = racs_mp3_encoder_encode(&enc, fmt, src, src_size, &ms);
    if (status != RACS_MP3_OK) {
        free(buf);
        racs_mp3_encoder_cleanup(&enc);
        return status;
    }

    *out = buf;
    *out_size = buf_size;

    racs_mp3_encoder_cleanup(&enc);
    return RACS_MP3_OK;
}
