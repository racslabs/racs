
#include "flac0.h"

void auxts_flac_open(auxts_flac* flac, void* data, size_t size) {
    memset(flac, 0, sizeof(auxts_flac));

    auxts_memory_stream_init(&flac->in_stream, data, size);
    FLAC__stream_decoder_init_stream(flac->decoder,
                                     auxts_flac_decoder_read_callback,
                                     NULL,
                                     NULL,
                                     NULL,
                                     NULL,
                                     auxts_flac_decoder_write_callback,
                                     auxts_flac_decoder_metadata_callback,
                                     auxts_flac_decoder_error_callback,
                                     flac);
}

FLAC__StreamDecoderReadStatus auxts_flac_decoder_read_callback(const FLAC__StreamDecoder* decoder,
                                                               FLAC__byte buffer[],
                                                               size_t* bytes,
                                                               void* client_data) {
    (void)decoder;

    auxts_flac* flac = (auxts_flac*)client_data;
    if (!flac) {
        perror("auxts_flac_block cannot be null");
        return FLAC__STREAM_DECODER_READ_STATUS_ABORT;
    }

    if (*bytes > flac->in_stream.size - flac->in_stream.current_pos)
        *bytes = flac->in_stream.size - flac->in_stream.current_pos;

    if (*bytes == 0)
        return FLAC__STREAM_DECODER_READ_STATUS_END_OF_STREAM;

    memcpy(buffer, flac->in_stream.data + flac->in_stream.current_pos, *bytes);
    flac->in_stream.current_pos += *bytes;

    return FLAC__STREAM_DECODER_READ_STATUS_CONTINUE;
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

    auxts_uint32 block_size = frame->header.blocksize;
    auxts_uint32 sample_size = flac->bit_depth == AUXTS_PCM_BIT_DEPTH_16 ?
            sizeof(auxts_int16) : sizeof(auxts_int32);

    size_t bytes = block_size * sample_size * flac->channels;
    if (bytes + flac->out_stream.current_pos > flac->total_samples)
        return FLAC__STREAM_DECODER_WRITE_STATUS_ABORT;

    auxts_pcm pcm;
    pcm.channels      = flac->channels;
    pcm.memory_stream = flac->out_stream;

    if (flac->bit_depth == AUXTS_PCM_BIT_DEPTH_16) {
        auxts_int16* buf = malloc(block_size * sample_size);

        auxts_simd_extract_s16(buffer[0], buf, block_size);
        auxts_pcm_write_s16i(&pcm, buf, block_size);

        free(buf);

        flac->out_stream = pcm.memory_stream;
        return FLAC__STREAM_DECODER_WRITE_STATUS_CONTINUE;
    }

    if (flac->bit_depth == AUXTS_PCM_BIT_DEPTH_24) {
        auxts_pcm_write_s32p(&pcm, buffer, block_size);

        flac->out_stream = pcm.memory_stream;
        return FLAC__STREAM_DECODER_WRITE_STATUS_CONTINUE;
    }

    return FLAC__STREAM_DECODER_WRITE_STATUS_ABORT;
}

void auxts_flac_decoder_metadata_callback(const FLAC__StreamDecoder *decoder,
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

    auxts_uint32 sample_size = flac->bit_depth == AUXTS_PCM_BIT_DEPTH_16 ?
            sizeof(auxts_int16) : sizeof(auxts_int32);

    size_t size = flac->channels * flac->total_samples * sample_size;

    void* buf = malloc(size);
    auxts_memory_stream_init(&flac->out_stream, buf, size);
}

void auxts_flac_decoder_error_callback(const FLAC__StreamDecoder *decoder,
                                       FLAC__StreamDecoderErrorStatus status,
                                       void *client_data) {
    (void)decoder;
    (void)client_data;

    fprintf(stderr, "FLAC decoding error: %s\n", FLAC__StreamDecoderErrorStatusString[status]);
}
