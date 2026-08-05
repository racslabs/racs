
#include "flac.h"


typedef struct {
    FLAC__StreamDecoder *dec;
    const racs_uint8 *src_buf;
    size_t src_size;
    size_t src_offset;
    racs_memstream *ms;
    racs_flac_format *fmt;
    int status;
} racs_flac_decoder;


FLAC__StreamDecoderReadStatus racs_flac_read_cb(const FLAC__StreamDecoder *decoder,
                                                FLAC__byte buffer[],
                                                size_t *bytes,
                                                void *data);

FLAC__StreamDecoderWriteStatus racs_flac_write_cb(const FLAC__StreamDecoder *decoder,
                                                  const FLAC__Frame *frame,
                                                  const FLAC__int32 *const buffer[],
                                                  void *data);

void racs_flac_error_cb(const FLAC__StreamDecoder *decoder,
                        FLAC__StreamDecoderErrorStatus status,
                        void *data);

int racs_flac_decoder_init(racs_flac_decoder *dec);

int racs_flac_decoder_decode(racs_flac_decoder *dec,
                             racs_flac_format *fmt,
                             const racs_uint8 *src,
                             size_t src_size,
                             racs_memstream *ms);

void racs_flac_decoder_cleanup(racs_flac_decoder *dec);


FLAC__StreamDecoderReadStatus racs_flac_read_cb(const FLAC__StreamDecoder *decoder,
                                                FLAC__byte buffer[],
                                                size_t *bytes,
                                                void *data) {
    (void) decoder;

    racs_flac_decoder *dec = (racs_flac_decoder *)data;
    if (dec->src_offset >= dec->src_size) {
        *bytes = 0;
        return FLAC__STREAM_DECODER_READ_STATUS_END_OF_STREAM;
    }

    size_t remaining = dec->src_size - dec->src_offset;
    if (*bytes > remaining) {
        *bytes = remaining;
    }

    memcpy(buffer, dec->src_buf + dec->src_offset, *bytes);
    dec->src_offset += *bytes;

    return FLAC__STREAM_DECODER_READ_STATUS_CONTINUE;
}

FLAC__StreamDecoderWriteStatus racs_flac_write_cb(const FLAC__StreamDecoder *decoder,
                                                  const FLAC__Frame *frame,
                                                  const FLAC__int32 *const buffer[],
                                                  void *data) {
    (void) decoder;

    racs_flac_decoder *dec = (racs_flac_decoder *)data;

    dec->fmt->channels = frame->header.channels;
    dec->fmt->sample_rate = frame->header.sample_rate;
    dec->fmt->bit_depth = frame->header.bits_per_sample;

    racs_uint32 channels = frame->header.channels;
    racs_uint32 samples = frame->header.blocksize;

    if (dec->fmt->bit_depth == 16) {
        racs_int16 *interleaved = malloc(samples * channels * sizeof(racs_int16));
        if (!interleaved) {
            dec->status = RACS_FLAC_ALLOC_ERROR;
            return FLAC__STREAM_DECODER_WRITE_STATUS_ABORT;
        }

        for (int sample = 0; sample < samples; sample++) {
            for (int channel = 0; channel < channels; channel++) {
                interleaved[sample * channels + channel] = (racs_int16)buffer[channel][sample];
            }
        }

        racs_memstream_write(dec->ms, interleaved, samples * channels * sizeof(racs_int16));
        free(interleaved);
    } else {
        dec->status = RACS_FLAC_UNSUPPORTED;
        return FLAC__STREAM_DECODER_WRITE_STATUS_ABORT;
    }

    return FLAC__STREAM_DECODER_WRITE_STATUS_CONTINUE;
}

void racs_flac_error_cb(const FLAC__StreamDecoder *decoder,
                        FLAC__StreamDecoderErrorStatus status,
                        void *data) {
    (void) decoder;
    (void) status;

    racs_flac_decoder *dec = (racs_flac_decoder *)data;
    dec->status = RACS_FLAC_DECODE_ERROR;
}

int racs_flac_decoder_init(racs_flac_decoder *dec) {
    dec->dec = FLAC__stream_decoder_new();
    if (!dec->dec) {
        return RACS_FLAC_ALLOC_ERROR;
    }

    FLAC__stream_decoder_set_md5_checking(dec->dec, false);

    FLAC__StreamDecoderInitStatus status = FLAC__stream_decoder_init_stream(
        dec->dec,
        racs_flac_read_cb,
        NULL,
        NULL,
        NULL,
        NULL,
        racs_flac_write_cb,
        NULL,
        racs_flac_error_cb,
        dec
    );

    if (status != FLAC__STREAM_DECODER_INIT_STATUS_OK) {
        FLAC__stream_decoder_delete(dec->dec);
        return RACS_FLAC_ALLOC_ERROR;
    }

    dec->status = RACS_FLAC_OK;
    return RACS_FLAC_OK;
}

int racs_flac_decoder_decode(racs_flac_decoder *dec,
                             racs_flac_format *fmt,
                             const racs_uint8 *src,
                             size_t src_size,
                             racs_memstream *ms) {
    dec->src_buf = src;
    dec->src_size = src_size;
    dec->src_offset = 0;
    dec->ms = ms;
    dec->fmt = fmt;

    FLAC__bool status = FLAC__stream_decoder_process_until_end_of_stream(dec->dec);

    if (!status || dec->status != RACS_FLAC_OK) {
        return RACS_FLAC_DECODE_ERROR;
    }

    return RACS_FLAC_OK;
}

void racs_flac_decoder_cleanup(racs_flac_decoder *dec) {
    if (dec && dec->dec) {
        FLAC__stream_decoder_finish(dec->dec);
        FLAC__stream_decoder_delete(dec->dec);
    }
}

int racs_flac_decode(racs_flac_format *fmt,
                     const racs_uint8 *src,
                     size_t src_size,
                     racs_uint8 **out,
                     size_t *out_size) {
    if (!src || src_size == 0 || !out || !out_size || !fmt) {
        return RACS_FLAC_PARAM_ERROR;
    }

    *out = NULL;
    *out_size = 0;

    size_t buf_size = 0;
    racs_uint8 *buf = malloc(1024);
    if (!buf) {
        return RACS_FLAC_ALLOC_ERROR;
    }

    racs_memstream ms = {
        .data = &buf,
        .size = &buf_size,
        .capacity = 1024,
    };

    racs_flac_decoder dec;
    int status = racs_flac_decoder_init(&dec);
    if (status != RACS_FLAC_OK) {
        free(buf);
        return status;
    }

    status = racs_flac_decoder_decode(&dec, fmt, src, src_size, &ms);
    if (status != RACS_FLAC_OK) {
        free(buf);
        racs_flac_decoder_cleanup(&dec);
        return status;
    }

    *out = buf;
    *out_size = buf_size;

    racs_flac_decoder_cleanup(&dec);
    return status;
}
