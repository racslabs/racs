#include "flac0.h"

void auxts_flac_open(auxts_flac* flac, void* in, size_t size) {
    memset(flac, 0, sizeof(auxts_flac));
    auxts_memory_stream_init(&flac->in_stream, in, size);
}

auxts_uint64 auxts_flac_read_pcm_s32(auxts_flac* flac, auxts_int32* in) {
    return auxts_flac_read_pcm(flac, in);
}

auxts_uint64 auxts_flac_read_pcm_s16(auxts_flac* flac, auxts_int16* in) {
    return auxts_flac_read_pcm(flac, in);
}

auxts_uint64 auxts_flac_read_pcm(auxts_flac* flac, void* in) {
    flac->out_stream.data = in;

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

int auxts_flac_decoder_write_pcm_s16(auxts_flac* flac, auxts_pcm* pcm, const auxts_int32* in, size_t n) {
    auxts_int16* buf = malloc(n * sizeof(auxts_int16));

    auxts_simd_extract_s16(in, buf, n);
    auxts_pcm_write_s16i(pcm, buf, n);

    free(buf);

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
    pcm.channels      = flac->channels;
    pcm.memory_stream = flac->out_stream;

    if (flac->bit_depth == AUXTS_PCM_BIT_DEPTH_16) {
        return auxts_flac_decoder_write_pcm_s16(flac, &pcm, buffer[0], block_size);
    }

    if (flac->bit_depth == AUXTS_PCM_BIT_DEPTH_24) {
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
    }

    auxts_uint32 sample_size = flac->bit_depth == AUXTS_PCM_BIT_DEPTH_16 ? sizeof(auxts_int16) : sizeof(auxts_int32);

    flac->out_stream.size = sample_size * flac->total_samples * flac->channels;
}

void auxts_flac_decoder_error_callback(const FLAC__StreamDecoder *decoder,
                                       FLAC__StreamDecoderErrorStatus status,
                                       void *client_data) {
    (void)decoder;
    (void)client_data;

    fprintf(stderr, "FLAC decoding error: %s\n", FLAC__StreamDecoderErrorStatusString[status]);
}

