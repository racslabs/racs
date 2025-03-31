
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
    if (flac->bit_depth == 16) {
//        auxts_pcm_init_s16(&pcm, (auxts_int16*)flac->out_stream.data, flac->out_stream.size);

    }

    return FLAC__STREAM_DECODER_WRITE_STATUS_CONTINUE;
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
        size = flac->channels * flac->samples_rate * sizeof(auxts_int16);
    } else {
        size = flac->channels * flac->samples_rate * sizeof(auxts_int32);
    }

    void* buf = malloc(size);
    auxts_memory_stream_init(&flac->out_stream, buf, size);
}
