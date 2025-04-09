#include "flac.h"

auxts_uint64 auxts_flac_read_pcm(auxts_flac* flac, void* in, size_t size) {
    memset(flac, 0, sizeof(auxts_flac));
    auxts_memory_stream_init(&flac->in_stream, in, size);

    flac->decoder = FLAC__stream_decoder_new();

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

    FLAC__stream_decoder_process_until_end_of_stream(flac->decoder);
    FLAC__stream_decoder_delete(flac->decoder);

    return flac->out_stream.current_pos;
}

auxts_uint64 auxts_flac_write_pcm_s16(auxts_flac* flac, auxts_int16* in, size_t samples) {
    auxts_int32* _in = malloc(samples * flac->channels * sizeof(auxts_int32));
    if (!_in) {
        perror("Failed to allocate data to auxts_flac");
        return 0;
    }

    auxts_simd_s16_s32(in, _in, samples * flac->channels);

    void* out = malloc(flac->block_size * flac->channels);
    if (!out) {
        perror("Failed to allocate data to auxts_flac");
        return 0;
    }

    auxts_memory_stream_init(&flac->out_stream, out, flac->block_size * flac->channels);

    flac->encoder = FLAC__stream_encoder_new();

    FLAC__stream_encoder_set_verify(flac->encoder, true);
    FLAC__stream_encoder_set_channels(flac->encoder, flac->channels);
    FLAC__stream_encoder_set_bits_per_sample(flac->encoder, flac->bit_depth);
    FLAC__stream_encoder_set_blocksize(flac->encoder, flac->block_size);
//    FLAC__stream_encoder_set_sample_rate(flac->encoder, 44100);
    FLAC__stream_encoder_set_compression_level(flac->encoder, flac->compression_level);

    int err;
    if ((err = FLAC__stream_encoder_init_stream(flac->encoder,
                                                auxts_flac_encoder_write_callback,
                                                NULL, NULL, NULL,
                                                flac)) != FLAC__STREAM_ENCODER_INIT_STATUS_OK) {
        fprintf(stderr, "FLAC encoding error: %s\n", FLAC__StreamEncoderInitStatusString[err]);
        return 0;
    }

    FLAC__stream_encoder_process_interleaved(flac->encoder, _in, samples);
    FLAC__stream_encoder_finish(flac->encoder);
    FLAC__stream_encoder_delete(flac->encoder);

    return flac->out_stream.current_pos;
}

auxts_uint64 auxts_flac_write_pcm_s32(auxts_flac* flac, auxts_int32* in, size_t samples) {
    void* out = malloc(flac->block_size * flac->channels);
    if (!out) {
        perror("Failed to allocate data to auxts_flac");
        return 0;
    }

    auxts_memory_stream_init(&flac->out_stream, out, flac->block_size * flac->channels);

    flac->encoder = FLAC__stream_encoder_new();

    FLAC__stream_encoder_set_verify(flac->encoder, true);
    FLAC__stream_encoder_set_channels(flac->encoder, flac->channels);
    FLAC__stream_encoder_set_bits_per_sample(flac->encoder, flac->bit_depth);
    FLAC__stream_encoder_set_blocksize(flac->encoder, flac->block_size);
    FLAC__stream_encoder_set_sample_rate(flac->encoder, flac->samples_rate);
    FLAC__stream_encoder_set_compression_level(flac->encoder, flac->compression_level);

    int err;
    if ((err = FLAC__stream_encoder_init_stream(flac->encoder,
                                                auxts_flac_encoder_write_callback,
                                                NULL, NULL, NULL,
                                                flac)) != FLAC__STREAM_ENCODER_INIT_STATUS_OK) {
        fprintf(stderr, "FLAC encoding error: %s\n", FLAC__StreamEncoderInitStatusString[err]);
        return 0;
    }

    FLAC__stream_encoder_process_interleaved(flac->encoder, in, samples);
    FLAC__stream_encoder_finish(flac->encoder);
    FLAC__stream_encoder_delete(flac->encoder);

    return flac->out_stream.current_pos;
}

FLAC__StreamEncoderWriteStatus auxts_flac_encoder_write_callback(const FLAC__StreamEncoder *encoder,
                                                                 const FLAC__byte buffer[],
                                                                 size_t bytes,
                                                                 unsigned samples,
                                                                 unsigned current_frame,
                                                                 void *client_data) {
    (void)encoder;

    auxts_flac* flac = (auxts_flac*)client_data;
    if (!flac) {
        perror("auxts_flac cannot be null");
        return FLAC__STREAM_ENCODER_WRITE_STATUS_FATAL_ERROR;
    }

    printf("bytes %zu\n", bytes);

    if (auxts_memory_stream_write(&flac->out_stream, buffer, bytes) == AUXTS_MEMORY_STREAM_ABORT) {
        perror("Error writing to auxts_memory_stream");
        return FLAC__STREAM_ENCODER_WRITE_STATUS_FATAL_ERROR;
    }

    return FLAC__STREAM_ENCODER_WRITE_STATUS_OK;
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

    if (*bytes > flac->in_stream.size - flac->in_stream.current_pos) {
        *bytes = flac->in_stream.size - flac->in_stream.current_pos;
    }

    if (*bytes == 0)
        return FLAC__STREAM_DECODER_READ_STATUS_END_OF_STREAM;

    memcpy(buffer, flac->in_stream.data + flac->in_stream.current_pos, *bytes);
    flac->in_stream.current_pos += *bytes;

    return FLAC__STREAM_DECODER_READ_STATUS_CONTINUE;
}

int auxts_flac_decoder_write_pcm_s16(auxts_flac* flac, auxts_pcm* pcm, const auxts_int32** in, size_t n) {
    auxts_int16* out1 = malloc(n * pcm->channels * sizeof(auxts_int16));

    for (int channel = 0; channel < pcm->channels; ++channel) {
        auxts_simd_s32_s16(in[channel], out1 + n * channel, n);
    }

    if (pcm->channels == AUXTS_PCM_CHANNELS_STEREO) {
        auxts_int16* out2 = malloc(n * pcm->channels * sizeof(auxts_int16));

        auxts_simd_interleave_s16(out1, out1 + n, out2, n);
        auxts_pcm_write_s16i(pcm, out2, n);

        free(out2);
    }

    free(out1);

    flac->out_stream = pcm->memory_stream;
    return FLAC__STREAM_DECODER_WRITE_STATUS_CONTINUE;
}

int auxts_flac_decoder_write_pcm_s32(auxts_flac* flac, auxts_pcm* pcm, const auxts_int32** in, size_t n) {
    auxts_pcm_write_s32p(pcm, in, n);

    flac->out_stream = pcm->memory_stream;
    return FLAC__STREAM_DECODER_WRITE_STATUS_CONTINUE;
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
    auxts_uint32 sample_size = flac->bit_depth == AUXTS_PCM_BIT_DEPTH_16 ? sizeof(auxts_int16) : sizeof(auxts_int32);

    size_t bytes = block_size * sample_size * flac->channels;
    if (bytes + flac->out_stream.current_pos > flac->total_samples)
        return FLAC__STREAM_DECODER_WRITE_STATUS_ABORT;

    auxts_pcm pcm;

    if (flac->bit_depth == AUXTS_PCM_BIT_DEPTH_16) {
        auxts_pcm_init_s16(&pcm, flac->channels, flac->samples_rate);
        return auxts_flac_decoder_write_pcm_s16(flac, &pcm, buffer, block_size);
    }

    if (flac->bit_depth == AUXTS_PCM_BIT_DEPTH_24) {
        auxts_pcm_init_s32(&pcm, flac->channels, flac->samples_rate);
        return auxts_flac_decoder_write_pcm_s32(flac, &pcm, buffer, block_size);
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
        flac->block_size    = metadata->data.stream_info.max_blocksize;

        printf("m %hhu\n", metadata->data.stream_info.sample_rate);
    }

    auxts_uint32 sample_size = flac->bit_depth == AUXTS_PCM_BIT_DEPTH_16 ? sizeof(auxts_int16) : sizeof(auxts_int32);
    size_t size = sample_size * flac->total_samples * flac->channels;

    void* out = malloc(size);
    if (!out) {
        perror("Failed to allocate data to auxts_flac");
        return;
    }

    auxts_memory_stream_init(&flac->out_stream, out, size);
}

void auxts_flac_decoder_error_callback(const FLAC__StreamDecoder *decoder,
                                       FLAC__StreamDecoderErrorStatus status,
                                       void *client_data) {
    (void)decoder;
    (void)client_data;

    fprintf(stderr, "FLAC decoding error: %s\n", FLAC__StreamDecoderErrorStatusString[status]);
}

void auxts_flac_destroy(auxts_flac* flac) {
    free(flac->out_stream.data);
}
