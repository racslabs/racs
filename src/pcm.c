#include "pcm.h"

void auxts_pcm_init_s32(auxts_pcm* pcm, auxts_uint8 channels, auxts_uint32 sample_rate) {
    memset(pcm, 0, sizeof(auxts_pcm));

    pcm->channels = channels;
    pcm->sample_rate = sample_rate;
    pcm->bit_depth = AUXTS_PCM_BIT_DEPTH_24;

    void* data = malloc(1024 * channels * sizeof(auxts_int32));
    if (!data) {
        perror("Failed to allocate data to auxts_pcm");
        return;
    }

    auxts_memory_stream_init(&pcm->memory_stream, data, 1024 * channels * sizeof(auxts_int32));
}

void auxts_pcm_init_s16(auxts_pcm* pcm, auxts_uint8 channels, auxts_uint32 sample_rate) {
    memset(pcm, 0, sizeof(auxts_pcm));

    pcm->channels = channels;
    pcm->sample_rate = sample_rate;
    pcm->bit_depth = AUXTS_PCM_BIT_DEPTH_16;

    void* data = malloc(1024 * channels * sizeof(auxts_int16));
    if (!data) {
        perror("Failed to allocate data to auxts_pcm");
        return;
    }

    auxts_memory_stream_init(&pcm->memory_stream, data, 1024 * channels * sizeof(auxts_int16));
}

void auxts_pcm_write_s32i(auxts_pcm* pcm, const auxts_int32* in, size_t sample_count) {
    auxts_memory_stream_write(&pcm->memory_stream, in, pcm->channels * sample_count * sizeof(auxts_int32));
}

void auxts_pcm_write_s16i(auxts_pcm* pcm, const auxts_int16* in, size_t sample_count) {
    auxts_memory_stream_write(&pcm->memory_stream, in, pcm->channels * sample_count * sizeof(auxts_int16));
}

void auxts_pcm_write_s32p(auxts_pcm* pcm, const auxts_int32** in, size_t sample_count) {
    size_t size = pcm->channels * sample_count * sizeof(auxts_int32);

    if (pcm->channels == AUXTS_PCM_CHANNELS_MONO) {
        auxts_memory_stream_write(&pcm->memory_stream, in[0], size);
        return;
    }

    if (pcm->channels == AUXTS_PCM_CHANNELS_STEREO) {
        auxts_int32* interleaved_samples = malloc(size);

        auxts_simd_interleave_s32(in[0], in[1], interleaved_samples, sample_count);
        auxts_memory_stream_write(&pcm->memory_stream, interleaved_samples, size);

        free(interleaved_samples);
        return;
    }
}

void auxts_pcm_write_s16p(auxts_pcm* pcm, const auxts_int16** in, size_t sample_count) {
    size_t size = pcm->channels * sample_count * sizeof(auxts_int16);

    if (pcm->channels == AUXTS_PCM_CHANNELS_MONO) {
        auxts_memory_stream_write(&pcm->memory_stream, in[0], size);
        return;
    }

    if (pcm->channels == AUXTS_PCM_CHANNELS_STEREO) {
        auxts_int16* interleaved_samples = malloc(size);

        auxts_simd_interleave_s16(in[0], in[1], interleaved_samples, sample_count);
        auxts_memory_stream_write(&pcm->memory_stream, interleaved_samples, size);

        free(interleaved_samples);
        return;
    }
}

void auxts_pcm_destroy(auxts_pcm* pcm) {
    free(pcm->memory_stream.data);
}
