
#include "flac0.h"

void auxts_flac_open(auxts_flac* flac, void* data, size_t size) {
    memset(flac, 0, sizeof(auxts_flac));

    auxts_memory_stream_init(&flac->in_stream, data, size);
    FLAC__stream_decoder_init_stream(flac->decoder,
                                     NULL,
                                     NULL,
                                     NULL,
                                     NULL,
                                     NULL,
                                     auxts_flac_decoder_write_callback,
                                     auxts_flac_metadata_callback,
                                     NULL,
                                     flac);
}

FLAC__StreamDecoderWriteStatus auxts_flac_decoder_write_callback(const FLAC__StreamDecoder *decoder,
                                                                 const FLAC__Frame *frame,
                                                                 const FLAC__int32 *const buffer[],
                                                                 void *client_data) {
    (void)decoder;

    auxts_flac* flac = (auxts_flac*)client_data;
    if (!flac) {
        perror("auxts_flac cannot be null");
        return FLAC__STREAM_DECODER_WRITE_STATUS_ABORT;
    }

    uint32_t block_size = frame->header.blocksize;
    if (block_size + flac->out_stream.current_pos > flac->total_samples)
        return FLAC__STREAM_DECODER_WRITE_STATUS_ABORT;

    auxts_pcm pcm;
    pcm.channels = flac->channels;
    pcm.bit_depth = flac->bit_depth;
    pcm.sample_rate = flac->samples_rate;
    pcm.memory_stream = flac->out_stream;

    if (flac->bit_depth == AUXTS_PCM_BIT_DEPTH_16) {
        auxts_int16* buf = malloc(block_size * sizeof(auxts_int16));

        auxts_simd_extract_s16(buffer[0], buf, block_size);
        auxts_pcm_write_s16i(&pcm, buf, block_size);

        free(buf);
        return FLAC__STREAM_DECODER_WRITE_STATUS_CONTINUE;
    }

    if (flac->bit_depth == AUXTS_PCM_BIT_DEPTH_24) {
        auxts_pcm_write_s32p(&pcm, buffer, block_size);
        return FLAC__STREAM_DECODER_WRITE_STATUS_CONTINUE;
    }

    return FLAC__STREAM_DECODER_WRITE_STATUS_ABORT;
}

void auxts_flac_metadata_callback(const FLAC__StreamDecoder *decoder,
                                  const FLAC__StreamMetadata *metadata,
                                  void *client_data) {
    (void)decoder;

    auxts_flac* flac = (auxts_flac*)client_data;
    if (!flac) {
        perror("auxts_flac cannot be null");
        return;
    }

    if (metadata->type == FLAC__METADATA_TYPE_STREAMINFO) {
        flac->channels      = metadata->data.stream_info.channels;
        flac->bit_depth     = metadata->data.stream_info.bits_per_sample;
        flac->samples_rate  = metadata->data.stream_info.sample_rate;
        flac->total_samples = metadata->data.stream_info.total_samples;
    }

    size_t size;
    if (flac->bit_depth == 16) {
        size = flac->channels * flac->total_samples * sizeof(auxts_int16);
    } else {
        size = flac->channels * flac->total_samples * sizeof(auxts_int32);
    }

    void* buf = malloc(size);
    auxts_memory_stream_init(&flac->out_stream, buf, size);
}
