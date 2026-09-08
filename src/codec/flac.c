
#include "flac.h"


#define RACS_FLAC_DEFAULT_COMPRESSION_LEVEL 5

#define RACS_FLAC_DEFAULT_FRAME_SIZE 4096


typedef struct {
    FLAC__StreamDecoder *dec;
    const racs_uint8 *src_buf;
    size_t src_size;
    size_t src_offset;
    racs_memstream *ms;
    racs_flac_format *fmt;
    int status;
} racs_flac_decoder;

typedef struct {
    FLAC__StreamEncoder *enc;
    racs_memstream *ms;
    int status;
} racs_flac_encoder;


FLAC__StreamDecoderWriteStatus racs_flac_decode_int16(racs_flac_decoder *dec,
                                                      const FLAC__int32 *const buffer[],
                                                      racs_uint32 channels,
                                                      racs_uint32 samples);

FLAC__StreamDecoderWriteStatus racs_flac_decode_int24(racs_flac_decoder *dec,
                                                      const FLAC__int32 *const buffer[],
                                                      racs_uint32 channels,
                                                      racs_uint32 samples);


FLAC__StreamDecoderReadStatus racs_flac_decode_read_cb(const FLAC__StreamDecoder *decoder,
                                                       FLAC__byte buffer[],
                                                       size_t *bytes,
                                                       void *data);

FLAC__StreamDecoderWriteStatus racs_flac_decode_write_cb(const FLAC__StreamDecoder *decoder,
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


FLAC__StreamEncoderWriteStatus racs_flac_encode_write_cb(const FLAC__StreamEncoder *encoder,
                                                         const FLAC__byte buffer[],
                                                         size_t bytes,
                                                         unsigned samples,
                                                         unsigned current_frame,
                                                         void *data);

FLAC__StreamEncoderSeekStatus racs_flac_encode_seek_cb(const FLAC__StreamEncoder *encoder,
                                                       FLAC__uint64 absolute_byte_offset,
                                                       void *data);

FLAC__StreamEncoderTellStatus racs_flac_encode_tell_cb(const FLAC__StreamEncoder *encoder,
                                                       FLAC__uint64 *absolute_byte_offset,
                                                       void *data);

int racs_flac_encoder_init(racs_flac_encoder *enc, racs_flac_format *fmt, racs_memstream *ms);

int racs_flac_encoder_encode(racs_flac_encoder *enc,
                             racs_flac_format *fmt,
                             const racs_uint8 *src,
                             size_t src_size);

void racs_flac_encoder_cleanup(racs_flac_encoder *enc);


FLAC__StreamDecoderWriteStatus racs_flac_decode_int16(racs_flac_decoder *dec,
                                                      const FLAC__int32 *const buffer[],
                                                      racs_uint32 channels,
                                                      racs_uint32 samples) {
    racs_int16 *flattend = malloc(samples * channels * sizeof(racs_int16));
    if (!flattend) {
        dec->status = RACS_FLAC_ALLOC_ERROR;
        return FLAC__STREAM_DECODER_WRITE_STATUS_ABORT;
    }

    for (int channel = 0; channel < channels; channel++) {
        racs_simd_int32_int16(buffer[channel], flattend + (samples * channel), samples);
    }

    if (channels == 2) {
        racs_int16 *interleaved = malloc(samples * channels * sizeof(racs_int16));
        if (!interleaved) {
            free(flattend);
            dec->status = RACS_FLAC_ALLOC_ERROR;
            return FLAC__STREAM_DECODER_WRITE_STATUS_ABORT;
        }

        racs_simd_interleave_int16(flattend, flattend + samples, interleaved, samples);
        racs_memstream_write(dec->ms, interleaved, samples * channels * sizeof(racs_int16));

        free(interleaved);
    } else if (channels == 1) {
        racs_memstream_write(dec->ms, flattend, samples * channels * sizeof(racs_int16));
    }

    free(flattend);

    return FLAC__STREAM_DECODER_WRITE_STATUS_CONTINUE;
}

FLAC__StreamDecoderWriteStatus racs_flac_decode_int24(racs_flac_decoder *dec,
                                                      const FLAC__int32 *const buffer[],
                                                      racs_uint32 channels,
                                                      racs_uint32 samples) {
    racs_int24 *flattend = malloc(samples * channels * sizeof(racs_int24));
    if (!flattend) {
        dec->status = RACS_FLAC_ALLOC_ERROR;
        return FLAC__STREAM_DECODER_WRITE_STATUS_ABORT;
    }

    for (racs_uint32 channel = 0; channel < channels; channel++) {
        racs_simd_int32_int24(buffer[channel], flattend + (samples * channel), samples);
    }

    if (channels == 2) {
        racs_int24 *interleaved = malloc(samples * channels * sizeof(racs_int24));
        if (!interleaved) {
            free(flattend);
            dec->status = RACS_FLAC_ALLOC_ERROR;
            return FLAC__STREAM_DECODER_WRITE_STATUS_ABORT;
        }

        racs_simd_interleave_int24(flattend, flattend + samples, interleaved, samples);
        racs_memstream_write(dec->ms, interleaved, samples * channels * sizeof(racs_int24));

        free(interleaved);
    } else if (channels == 1) {
        racs_memstream_write(dec->ms, flattend, samples * channels * sizeof(racs_int24));
    }

    free(flattend);

    return FLAC__STREAM_DECODER_WRITE_STATUS_CONTINUE;
}

FLAC__StreamDecoderReadStatus racs_flac_decode_read_cb(const FLAC__StreamDecoder *decoder,
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

FLAC__StreamDecoderWriteStatus racs_flac_decode_write_cb(const FLAC__StreamDecoder *decoder,
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

    if (channels != 1 && channels != 2) {
        dec->status = RACS_FLAC_UNSUPPORTED;
        return FLAC__STREAM_DECODER_WRITE_STATUS_ABORT;
    }

    if (dec->fmt->bit_depth == 16) {
        return racs_flac_decode_int16(dec, buffer, channels, samples);
    } else if (dec->fmt->bit_depth == 24) {
        return racs_flac_decode_int24(dec, buffer, channels, samples);
    } 

    dec->status = RACS_FLAC_UNSUPPORTED;
    return FLAC__STREAM_DECODER_WRITE_STATUS_ABORT;
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
        racs_flac_decode_read_cb,
        NULL,
        NULL,
        NULL,
        NULL,
        racs_flac_decode_write_cb,
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
    
    return dec->status;
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
        .offset = 0
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

FLAC__StreamEncoderWriteStatus racs_flac_encode_write_cb(const FLAC__StreamEncoder *encoder,
                                                         const FLAC__byte buffer[],
                                                         size_t bytes,
                                                         unsigned samples,
                                                         unsigned current_frame,
                                                         void *data) {
    (void) encoder;
    (void) samples;
    (void) current_frame;

    racs_flac_encoder *enc = (racs_flac_encoder *)data;
    racs_memstream_write(enc->ms, buffer, bytes);

    return FLAC__STREAM_ENCODER_WRITE_STATUS_OK;
}

FLAC__StreamEncoderSeekStatus racs_flac_encode_seek_cb(const FLAC__StreamEncoder *encoder,
                                                       FLAC__uint64 absolute_byte_offset,
                                                       void *data) {
    (void) encoder;
    racs_flac_encoder *enc = (racs_flac_encoder *)data;

    if (racs_memstream_seek(enc->ms, absolute_byte_offset) != 0) {
        return FLAC__STREAM_ENCODER_SEEK_STATUS_ERROR;
    }

    return FLAC__STREAM_ENCODER_SEEK_STATUS_OK;
}

FLAC__StreamEncoderTellStatus racs_flac_encode_tell_cb(const FLAC__StreamEncoder *encoder,
                                                       FLAC__uint64 *absolute_byte_offset,
                                                       void *data) {
    (void)encoder;

    racs_flac_encoder *enc = (racs_flac_encoder *)data;

    FLAC__int64 offset = racs_memstream_tell(enc->ms);
    if (offset < 0) {
        return FLAC__STREAM_ENCODER_TELL_STATUS_ERROR;
    }

    *absolute_byte_offset = (FLAC__uint64)offset;
    return FLAC__STREAM_ENCODER_TELL_STATUS_OK;
}

int racs_flac_encoder_init(racs_flac_encoder *enc, racs_flac_format *fmt, racs_memstream *ms) {
    if (fmt->channels != 1 && fmt->channels != 2) {
        return RACS_FLAC_UNSUPPORTED;
    }

    if (fmt->bit_depth != 16 && fmt->bit_depth != 24) {
        return RACS_FLAC_UNSUPPORTED;
    }

    enc->enc = FLAC__stream_encoder_new();
    if (!enc->enc) {
        return RACS_FLAC_ALLOC_ERROR;
    }

    enc->ms = ms;
    enc->status = RACS_FLAC_OK;

    FLAC__stream_encoder_set_channels(enc->enc, fmt->channels);
    FLAC__stream_encoder_set_bits_per_sample(enc->enc, fmt->bit_depth);
    FLAC__stream_encoder_set_sample_rate(enc->enc, fmt->sample_rate);
    FLAC__stream_encoder_set_compression_level(enc->enc, RACS_FLAC_DEFAULT_COMPRESSION_LEVEL);

    FLAC__StreamEncoderInitStatus status = FLAC__stream_encoder_init_stream(
        enc->enc,
        racs_flac_encode_write_cb,
        racs_flac_encode_seek_cb,
        racs_flac_encode_tell_cb,
        NULL,
        enc
    );

    if (status != FLAC__STREAM_ENCODER_INIT_STATUS_OK) {
        FLAC__stream_encoder_delete(enc->enc);
        return RACS_FLAC_ALLOC_ERROR;
    }

    return RACS_FLAC_OK;
}

int racs_flac_encoder_encode(racs_flac_encoder *enc,
                             racs_flac_format *fmt,
                             const racs_uint8 *src,
                             size_t src_size) {
    if (!enc || !enc->enc || !src || !fmt || fmt->channels <= 0) {
        return RACS_FLAC_PARAM_ERROR;
    }

    if (fmt->bit_depth != 16 && fmt->bit_depth != 24) {
        return RACS_FLAC_UNSUPPORTED;
    }

    size_t bytes_per_sample = (fmt->bit_depth == 16) ? sizeof(racs_int16) : sizeof(racs_int24);
    size_t total_samples = src_size / bytes_per_sample;
    size_t frames_remaining = total_samples / fmt->channels;  

    const racs_uint8 *in_ptr = src;
    size_t total_frame_samples = RACS_FLAC_DEFAULT_FRAME_SIZE * fmt->channels;

    FLAC__int32 *buf = malloc(total_frame_samples * sizeof(FLAC__int32));
    if (!buf) {
        return RACS_FLAC_ALLOC_ERROR;
    }

    while (frames_remaining > 0) {
        unsigned chunk_frames = (frames_remaining > (size_t)RACS_FLAC_DEFAULT_FRAME_SIZE) ?
                                RACS_FLAC_DEFAULT_FRAME_SIZE : (unsigned)frames_remaining;

        size_t chunk_samples = (size_t)chunk_frames * fmt->channels;

        if (fmt->bit_depth == 16) {
            racs_simd_int16_int32((const racs_int16 *) in_ptr, buf, chunk_samples);
        }
        
        if (fmt->bit_depth == 24) {
            racs_simd_int24_int32((const racs_int24 *) in_ptr, buf, chunk_samples);
        }

        FLAC__bool status = FLAC__stream_encoder_process_interleaved(enc->enc, buf, chunk_frames);
        if (!status) {
            free(buf);
            return RACS_FLAC_ENCODE_ERROR;
        }

        frames_remaining -= chunk_frames;
        in_ptr += chunk_samples * bytes_per_sample;
    }

    free(buf);
    return RACS_FLAC_OK;
}


void racs_flac_encoder_cleanup(racs_flac_encoder *enc) {
    if (enc && enc->enc) {
        FLAC__stream_encoder_finish(enc->enc);
        FLAC__stream_encoder_delete(enc->enc);
    }
}

int racs_flac_encode(racs_flac_format *fmt,
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
        .offset = 0
    };

    racs_flac_encoder enc;
    int status = racs_flac_encoder_init(&enc, fmt, &ms);
    if (status != RACS_FLAC_OK) {
        free(buf);
        return status;
    }

    status = racs_flac_encoder_encode(&enc, fmt, src, src_size);
    if (status != RACS_FLAC_OK) {
        free(buf);
        racs_flac_encoder_cleanup(&enc);
        return status;
    }

    racs_flac_encoder_cleanup(&enc);

    *out = buf;
    *out_size = buf_size;

    return RACS_FLAC_OK;
}
