
#include "opus.h"


#define RACS_OPUS_MAX_FRAME_SIZE 5760

#define RACS_OPUS_DEFAULT_FRAME_SIZE 960


typedef struct {
    OggOpusFile *of;
    racs_int16 *pcm_block;
    size_t pcm_block_size;
} racs_opus_decoder;

typedef struct {
    OggOpusEnc *enc;
    OggOpusComments *comments;
    racs_memstream *ms;
} racs_opus_encoder;


int racs_opus_decoder_init(racs_opus_decoder *dec,
                           racs_opus_format *fmt,
                           const racs_uint8 *src,
                           size_t src_size);

int racs_opus_decoder_decode(racs_opus_decoder *dec,
                             racs_opus_format *fmt,
                             racs_memstream *ms);

void racs_opus_decoder_cleanup(racs_opus_decoder *dec);

int racs_opus_close_cb(void *data);

int racs_opus_write_cb(void *data, const racs_uint8 *ptr, racs_int32 len);

int racs_opus_encoder_init(racs_opus_encoder *enc, racs_opus_format *fmt);

int racs_opus_encoder_cleanup(racs_opus_encoder *enc);


int racs_opus_encoder_encode(racs_opus_encoder *enc,
                            racs_opus_format *fmt,
                            const racs_uint8 *src,
                            size_t src_size,
                            racs_memstream *ms);


int racs_opus_decoder_init(racs_opus_decoder *dec,
                           racs_opus_format *fmt,
                           const racs_uint8 *src,
                           size_t src_size) {
    int err = 0;

    dec->of = op_open_memory(src, src_size, &err);
    if (!dec->of) {
        return RACS_OPUS_ALLOC_ERROR;
    }

    const OpusHead *head = op_head(dec->of, -1);
    if (!head) {
        op_free(dec->of);
        return RACS_OPUS_DECODE_ERROR;
    }

    fmt->channels = head->channel_count;
    fmt->sample_rate = 48000;

    dec->pcm_block_size = RACS_OPUS_MAX_FRAME_SIZE * fmt->channels * sizeof(racs_int16);
    dec->pcm_block = malloc(dec->pcm_block_size);
    if (!dec->pcm_block) {
        op_free(dec->of);
        return RACS_OPUS_ALLOC_ERROR;
    }

    return RACS_OPUS_OK;
}

int racs_opus_decoder_decode(racs_opus_decoder *dec,
                             racs_opus_format *fmt,
                             racs_memstream *ms) {
    while (1) {
        int samples_decoded = op_read(dec->of, dec->pcm_block, RACS_OPUS_MAX_FRAME_SIZE * fmt->channels, NULL);

        if (samples_decoded == 0) {
            break;
        }

        if (samples_decoded < 0) {
            return RACS_OPUS_DECODE_ERROR;
        }

        size_t bytes_decoded = samples_decoded * fmt->channels * sizeof(racs_int16);

        int status = racs_memstream_write(ms, dec->pcm_block, bytes_decoded);
        if (status < 0) {
            return RACS_OPUS_DECODE_ERROR;
        }
    }

    return RACS_OPUS_OK;
}

void racs_opus_decoder_cleanup(racs_opus_decoder *dec) {
    if (!dec) {
        return;
    }

    if (dec->pcm_block) {
        free(dec->pcm_block);
        dec->pcm_block = NULL;
    }

    if (dec->of) {
        op_free(dec->of);
        dec->of = NULL;
    }
}

int racs_opus_decode(racs_opus_format *fmt,
                     const racs_uint8 *src,
                     size_t src_size,
                     racs_uint8 **out,
                     size_t *out_size) {
    if (!src || src_size == 0 || !out || !out_size || !fmt) {
        return RACS_OPUS_PARAM_ERROR;
    }

    *out = NULL;
    *out_size = 0;

    size_t buf_size = 0;
    racs_uint8 *buf = malloc(1024);
    if (!buf) return RACS_OPUS_ALLOC_ERROR;

    racs_memstream ms = {
        .data = &buf,
        .size = &buf_size,
        .capacity = 1024,
        .offset = 0
    };

    racs_opus_decoder dec;
    int status = racs_opus_decoder_init(&dec, fmt, src, src_size);
    if (status != RACS_OPUS_OK) {
        free(buf);
        return status;
    }

    status = racs_opus_decoder_decode(&dec, fmt, &ms);
    if (status != RACS_OPUS_OK) {
        free(buf);
        racs_opus_decoder_cleanup(&dec);
        return status;
    }

    *out = buf;
    *out_size = buf_size;

    racs_opus_decoder_cleanup(&dec);
    return RACS_OPUS_OK;
}

int racs_opus_close_cb(void *data) {
    (void) data;
    return OPE_OK;
}

int racs_opus_write_cb(void *data, const racs_uint8 *ptr, racs_int32 len) {
    racs_opus_encoder *enc = (racs_opus_encoder *)data;
    racs_memstream_write(enc->ms, ptr, len);

    return OPE_OK;
}

int racs_opus_encoder_init(racs_opus_encoder *enc, racs_opus_format *fmt) {
    int err = OPE_OK;

    OpusEncCallbacks callbacks = {
        .write = racs_opus_write_cb,
        .close = racs_opus_close_cb
    };

    enc->comments = ope_comments_create();
    if (!enc->comments) {
        return RACS_OPUS_ALLOC_ERROR;
    }

    enc->enc = ope_encoder_create_callbacks(&callbacks, enc, enc->comments, (int) fmt->sample_rate,
                                            fmt->channels, 0, &err);

    if (!enc->enc) {
        ope_comments_destroy(enc->comments);
        return RACS_OPUS_ALLOC_ERROR;
    }

    return RACS_OPUS_OK;
}

int racs_opus_encoder_encode(racs_opus_encoder *enc,
                             racs_opus_format *fmt,
                             const racs_uint8 *src,
                             size_t src_size,
                             racs_memstream *ms) {
    if (!enc || !enc->enc || !src || !fmt || fmt->channels <= 0) {
        return RACS_OPUS_PARAM_ERROR;
    }

    if (fmt->bit_depth != 16) {
        return RACS_OPUS_UNSUPPORTED;
    }

    size_t total_samples = src_size / sizeof(racs_int16);
    size_t samples_remaining = total_samples / fmt->channels;

    if (samples_remaining <= 0) {
        return RACS_OPUS_OK;
    }

    const opus_int16 *in_ptr = (const opus_int16 *)src;

    while (samples_remaining > 0) {
        int chunk_size = (int)((samples_remaining > RACS_OPUS_DEFAULT_FRAME_SIZE) ?
                            RACS_OPUS_DEFAULT_FRAME_SIZE : samples_remaining);

        int err = ope_encoder_write(enc->enc, in_ptr, chunk_size);
        if (err != OPE_OK) {
            return RACS_OPUS_ENCODE_ERROR;
        }

        samples_remaining -= chunk_size;
        in_ptr += ((size_t)chunk_size * fmt->channels);
    }

    return RACS_OPUS_OK;
}

int racs_opus_encoder_cleanup(racs_opus_encoder *enc) {
    if (!enc) {
        return RACS_OPUS_PARAM_ERROR;
    }

    if (enc->enc) {
        ope_encoder_drain(enc->enc);
        ope_encoder_destroy(enc->enc);
        enc->enc = NULL;
    }

    if (enc->comments) {
        ope_comments_destroy(enc->comments);
        enc->comments = NULL;
    }

    return RACS_OPUS_OK;
}

int racs_opus_encode(racs_opus_format *fmt,
                     const racs_uint8 *src,
                     size_t src_size,
                     racs_uint8 **out,
                     size_t *out_size) {
    if (!src || src_size == 0 || !out || !out_size || !fmt) {
        return RACS_OPUS_PARAM_ERROR;
    }

    *out = NULL;
    *out_size = 0;

    size_t buf_size = 0;
    racs_uint8 *buf = malloc(1024);
    if (!buf) {
        return RACS_OPUS_ALLOC_ERROR;
    }

    racs_memstream ms = {
        .data = &buf,
        .size = &buf_size,
        .capacity = 1024,
        .offset = 0
    };

    racs_opus_encoder enc;
    int status = racs_opus_encoder_init(&enc, fmt);
    if (status != RACS_OPUS_OK) {
        free(buf);
        return status;
    }

    status = racs_opus_encoder_encode(&enc, fmt, src, src_size, &ms);
    if (status != RACS_OPUS_OK) {
        free(buf);
        racs_opus_encoder_cleanup(&enc);
        return status;
    }

    *out = buf;
    *out_size = buf_size;

    racs_opus_encoder_cleanup(&enc);
    return RACS_OPUS_OK;
}
