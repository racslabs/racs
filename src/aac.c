
#include "aac.h"


typedef struct {
    HANDLE_AACDECODER hdec;
    racs_int16 *pcm_block;
    size_t pcm_block_size;
} racs_aac_decoder;


int racs_aac_decoder_init(racs_aac_decoder *dec);

int racs_aac_decoder_decode(racs_aac_decoder *dec,
                            racs_aac_format *fmt,
                            const racs_uint8 *src,
                            size_t src_size,
                            racs_memstream *ms);

void racs_aac_decoder_cleanup(racs_aac_decoder *dec);


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
    size_t bytes_left = src_size;
    const UCHAR *src_ptr = (const UCHAR *)src;

    while (bytes_left > 0) {
        UINT bytes_valid = (UINT)bytes_left;
        AAC_DECODER_ERROR err = aacDecoder_Fill(dec->hdec, (UCHAR **)&src_ptr, &bytes_valid, &bytes_valid);

        bytes_left = bytes_valid;

        if (err != AAC_DEC_OK) {
            return RACS_AAC_DECODE_ERROR;
        }

        err = aacDecoder_DecodeFrame(dec->hdec, dec->pcm_block, dec->pcm_block_size / sizeof(racs_int16), 0);
        if (err == AAC_DEC_NOT_ENOUGH_BITS) {
            if (bytes_left == 0) {
                break;
            }

            continue;
        }

        if (err != AAC_DEC_OK) {
            return RACS_AAC_DECODE_ERROR;
        }

        CStreamInfo *info = aacDecoder_GetStreamInfo(dec->hdec);
        fmt->channels    = info->numChannels;
        fmt->sample_rate = info->sampleRate;
        fmt->bit_depth   = RACS_AAC_BIT_DEPTH;

        size_t frame_samples = (size_t)info->frameSize * info->numChannels;
        racs_memstream_write(ms, dec->pcm_block, frame_samples * sizeof(racs_int16));
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
