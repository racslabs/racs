
#include "aac.h"
#include <stdio.h>

typedef struct {
    HANDLE_AACDECODER hdec;
    racs_int16 *pcm_block;
    size_t pcm_block_size;
} racs_aac_decoder;

typedef struct {
    HANDLE_AACENCODER henc;
    INT frame_size;
    INT max_out_size;
} racs_aac_encoder;


int racs_aac_decoder_init(racs_aac_decoder *dec);

int racs_aac_decoder_decode(racs_aac_decoder *dec,
                            racs_aac_format *fmt,
                            const racs_uint8 *src,
                            size_t src_size,
                            racs_memstream *ms);

void racs_aac_decoder_cleanup(racs_aac_decoder *dec);

int racs_aac_encoder_init(racs_aac_encoder *enc, racs_aac_format *fmt);

int racs_aac_encode_helper(HANDLE_AACENCODER henc,
                           void *in_ptr, INT in_size, INT in_samples,
                           void *out_ptr, INT out_size,
                           AACENC_OutArgs *out_args);

int racs_aac_encoder_encode(racs_aac_encoder *enc,
                            racs_aac_format *fmt,
                            const racs_uint8 *src,
                            size_t src_size,
                            racs_memstream *ms);

void racs_aac_encoder_cleanup(racs_aac_encoder *enc);


int racs_aac_decoder_init(racs_aac_decoder *dec) {
    dec->hdec = aacDecoder_Open(TT_MP4_ADTS, 1);
    if (!dec->hdec) {
        return RACS_AAC_ALLOC_ERROR;
    }

    dec->pcm_block_size = 2048 * 2 * sizeof(racs_int16);
    dec->pcm_block = malloc(dec->pcm_block_size);
    if (!dec->pcm_block) {
        aacDecoder_Close(dec->hdec);
        return RACS_AAC_ALLOC_ERROR;
    }

    return RACS_AAC_OK;
}

int racs_aac_decoder_decode(racs_aac_decoder *dec,
                            racs_aac_format *fmt,
                            const racs_uint8 *src,
                            size_t src_size,
                            racs_memstream *ms) {
    size_t remaining = src_size;
    const UCHAR *src_ptr = (const UCHAR *)src;

    while (remaining > 0) {
        UINT bytes_valid = (UINT)remaining;
        UINT buf_size = bytes_valid;

        UCHAR *in_ptr = (UCHAR *)src_ptr;
        AAC_DECODER_ERROR err = aacDecoder_Fill(dec->hdec, &in_ptr, &buf_size, &bytes_valid);

        if (err != AAC_DEC_OK) {
            return RACS_AAC_DECODE_ERROR;
        }

        size_t bytes_read = remaining - bytes_valid;
        src_ptr += bytes_read;
        remaining = bytes_valid;

        while (1) {
            err = aacDecoder_DecodeFrame(dec->hdec, dec->pcm_block, dec->pcm_block_size / sizeof(racs_int16), 0);
            if (err == AAC_DEC_NOT_ENOUGH_BITS) {
                break;
            }

            if (err != AAC_DEC_OK) {
                return RACS_AAC_DECODE_ERROR;
            }

            CStreamInfo *info = aacDecoder_GetStreamInfo(dec->hdec);
            fmt->channels    = info->numChannels;
            fmt->sample_rate = info->sampleRate;
            fmt->bit_depth   = RACS_AAC_BIT_DEPTH;

            if (fmt->channels != 1 && fmt->channels != 2) {
                return RACS_AAC_UNSUPPORTED;
            }

            size_t frame_samples = (size_t)info->frameSize * info->numChannels;
            racs_memstream_write(ms, dec->pcm_block, frame_samples * sizeof(racs_int16));
        }
    }

    return RACS_AAC_OK;
}

void racs_aac_decoder_cleanup(racs_aac_decoder *dec) {
    if (!dec) {
        return;
    }

    if (dec->pcm_block) {
        free(dec->pcm_block);
    }

    if (dec->hdec) {
        aacDecoder_Close(dec->hdec);
    }
}

int racs_aac_decode(racs_aac_format *fmt,
                    const racs_uint8 *src,
                    size_t src_size,
                    racs_uint8 **out,
                    size_t *out_size) {
    if (!src || src_size == 0 || !out || !out_size || !fmt) {
        return RACS_AAC_PARAM_ERROR;
    }

    *out = NULL;
    *out_size = 0;

    size_t buf_size = 0;
    racs_uint8 *buf = malloc(1024);
    if (!buf) {
        return RACS_AAC_ALLOC_ERROR;
    }

    racs_memstream ms = {
        .data = &buf,
        .size = &buf_size,
        .capacity = 1024,
        .offset = 0
    };

    racs_aac_decoder dec;
    int status = racs_aac_decoder_init(&dec);
    if (status != RACS_AAC_OK) {
        free(buf);
        return status;
    }

    status = racs_aac_decoder_decode(&dec, fmt, src, src_size, &ms);
    if (status != RACS_AAC_OK) {
        free(buf);
        racs_aac_decoder_cleanup(&dec);
        return status;
    }

    *out = buf;
    *out_size = buf_size;

    racs_aac_decoder_cleanup(&dec);
    return status;
}

int racs_aac_encoder_init(racs_aac_encoder *enc, racs_aac_format *fmt) {
    enc->henc = NULL;

    if (fmt->channels != 1 && fmt-> channels != 2) {
        return RACS_AAC_UNSUPPORTED;
    }

    if (aacEncOpen(&enc->henc, 0, fmt->channels) != AACENC_OK) {
        return RACS_AAC_ALLOC_ERROR;
    }

    aacEncoder_SetParam(enc->henc, AACENC_AOT, 2);
    aacEncoder_SetParam(enc->henc, AACENC_SAMPLERATE, fmt->sample_rate);
    aacEncoder_SetParam(enc->henc, AACENC_CHANNELMODE, fmt->channels);
    aacEncoder_SetParam(enc->henc, AACENC_BITRATE, 128000);   // 128 kbps
    aacEncoder_SetParam(enc->henc, AACENC_TRANSMUX, 2);

    if (aacEncEncode(enc->henc, NULL, NULL, NULL, NULL) != AACENC_OK) {
        aacEncClose(&enc->henc);
        return RACS_AAC_ALLOC_ERROR;
    }

    AACENC_InfoStruct info = {0};
    aacEncInfo(enc->henc, &info);

    enc->frame_size = (INT)info.frameLength * fmt->channels;
    enc->max_out_size = (INT)info.maxOutBufBytes;

    return RACS_AAC_OK;
}

int racs_aac_encode_helper(HANDLE_AACENCODER henc,
                           void *in_ptr, INT in_size, INT in_samples,
                           void *out_ptr, INT out_size,
                           AACENC_OutArgs *out_args) {
    INT in_buf_id = IN_AUDIO_DATA;
    INT in_element_size = sizeof(racs_int16);

    AACENC_BufDesc in_desc = {
        .numBufs = 1, .bufs = &in_ptr, .bufferIdentifiers = &in_buf_id,
        .bufSizes = &in_size, .bufElSizes = &in_element_size
    };

    INT out_buf_id = OUT_BITSTREAM_DATA;
    INT out_element_size = sizeof(racs_uint8);

    AACENC_BufDesc out_desc = {
        .numBufs = 1, .bufs = &out_ptr, .bufferIdentifiers = &out_buf_id,
        .bufSizes = &out_size, .bufElSizes = &out_element_size
    };

    AACENC_InArgs in_args = { .numInSamples = in_samples };
    return aacEncEncode(henc, &in_desc, &out_desc, &in_args, out_args);
}

int racs_aac_encoder_encode(racs_aac_encoder *enc,
                            racs_aac_format *fmt,
                            const racs_uint8 *src,
                            size_t src_size,
                            racs_memstream *ms) {

    racs_uint8 *out_ptr = malloc(enc->max_out_size);
    if (!out_ptr) {
        return RACS_AAC_ALLOC_ERROR;
    }

    size_t samples_read = 0;
    size_t total_samples = src_size / sizeof(racs_int16);

    racs_int16 *samples = (racs_int16 *)src;
    AACENC_OutArgs out_args = {0};

    while (samples_read + enc->frame_size <= total_samples) {
        void *in_ptr = (void *)(samples + samples_read);
        INT in_size = enc->frame_size * sizeof(racs_int16);

        if (racs_aac_encode_helper(enc->henc, in_ptr, in_size, enc->frame_size,
                            out_ptr, enc->max_out_size, &out_args) != AACENC_OK) {
            free(out_ptr);
            return RACS_AAC_ENCODE_ERROR;
        }

        samples_read += out_args.numInSamples;

        if (out_args.numOutBytes > 0) {
            racs_memstream_write(ms, out_ptr, out_args.numOutBytes);
        }
    }

    size_t remaining = total_samples - samples_read;
    if (remaining > 0) {
        racs_int16 *in_ptr = calloc(enc->frame_size, sizeof(racs_int16));
        if (!in_ptr) {
            free(out_ptr);
            return RACS_AAC_ALLOC_ERROR;
        }

        memcpy(in_ptr, samples + samples_read, remaining * sizeof(racs_int16));
        INT in_size = enc->frame_size * sizeof(racs_int16);

        AACENC_ERROR err = racs_aac_encode_helper(enc->henc, in_ptr, in_size, enc->frame_size,
                                           out_ptr, enc->max_out_size, &out_args);
        free(in_ptr);

        if (err != AACENC_OK) {
            free(out_ptr);
            return RACS_AAC_ENCODE_ERROR;
        }

        if (out_args.numOutBytes > 0) {
            racs_memstream_write(ms, out_ptr, out_args.numOutBytes);
        }
    }

    while (1) {
        AACENC_ERROR err = racs_aac_encode_helper(enc->henc, NULL, 0, -1,
                                           out_ptr, enc->max_out_size, &out_args);

        if (err != AACENC_OK) {
            break;
        }

        if (out_args.numOutBytes > 0) {
            racs_memstream_write(ms, out_ptr, out_args.numOutBytes);
        }
    }

    free(out_ptr);
    return RACS_AAC_OK;
}


void racs_aac_encoder_cleanup(racs_aac_encoder *enc) {
    if (enc->henc) {
        aacEncClose(&enc->henc);
    }
}

int racs_aac_encode(racs_aac_format *fmt,
                    const racs_uint8 *src,
                    size_t src_size,
                    racs_uint8 **out,
                    size_t *out_size) {
    if (!src || src_size == 0 || !out || !out_size || !fmt) {
        return RACS_AAC_PARAM_ERROR;
    }

    *out = NULL;
    *out_size = 0;

    size_t buf_size = 0;
    racs_uint8 *buf = malloc(1024);
    if (!buf) {
        return RACS_AAC_ALLOC_ERROR;
    }

    racs_memstream ms = {
        .data = &buf,
        .size = &buf_size,
        .capacity = 1024,
        .offset = 0
    };

    racs_aac_encoder enc;
    int status = racs_aac_encoder_init(&enc, fmt);
    if (status != RACS_AAC_OK) {
        free(buf);
        return status;
    }

    status = racs_aac_encoder_encode(&enc, fmt, src, src_size, &ms);
    if (status != RACS_AAC_OK) {
        free(buf);
        racs_aac_encoder_cleanup(&enc);
        return status;
    }

    *out = buf;
    *out_size = buf_size;

    racs_aac_encoder_cleanup(&enc);
    return RACS_AAC_OK;
}