#include "pcm.h"

void auxts_pcm_init_s32(auxts_pcm* pcm, auxts_int32* buf, auxts_uint64 n) {
    memset(pcm, 0, sizeof(auxts_pcm));
    auxts_memory_stream_init(&pcm->memory_stream, buf, n * sizeof(auxts_int32));
}

void auxts_pcm_init_s16(auxts_pcm* pcm, auxts_int16* buf, auxts_uint64 n) {
    memset(pcm, 0, sizeof(auxts_pcm));
    auxts_memory_stream_init(&pcm->memory_stream, buf, n * sizeof(auxts_int16));
}

void auxts_pcm_write_s32i(auxts_pcm* pcm, auxts_int32* samples, auxts_uint64 n) {
    auxts_memory_stream_write(&pcm->memory_stream, samples, n * sizeof(auxts_int32));
}

void auxts_pcm_write_s16i(auxts_pcm* pcm, auxts_int16* samples, auxts_uint64 n) {
    auxts_memory_stream_write(&pcm->memory_stream, samples, n * sizeof(auxts_int16));
}

void auxts_pcm_write_s32p(auxts_pcm* pcm, auxts_int32** samples, auxts_uint64 n) {
    if (pcm->channels == AUXTS_PCM_CHANNELS_MONO) {
        auxts_memory_stream_write(&pcm->memory_stream, samples, n * sizeof(auxts_int32));
        return;
    }

    if (pcm->channels == AUXTS_PCM_CHANNELS_STEREO) {
        size_t m = pcm->channels * n * sizeof(auxts_int32);

        auxts_int32* interleaved_samples = malloc(m);
        auxts_simd_interleave_s32(samples[0], samples[1], interleaved_samples, m);
        auxts_memory_stream_write(&pcm->memory_stream, interleaved_samples, m);

        free(interleaved_samples);
        return;
    }
}

void auxts_pcm_write_s16p(auxts_pcm* pcm, auxts_int16** samples, auxts_uint64 n) {
    if (pcm->channels == AUXTS_PCM_CHANNELS_MONO) {
        auxts_memory_stream_write(&pcm->memory_stream, samples, n * sizeof(auxts_int16));
        return;
    }

    if (pcm->channels == AUXTS_PCM_CHANNELS_STEREO) {
        size_t m = pcm->channels * n * sizeof(auxts_int16);

        auxts_int16* interleaved_samples = malloc(m);
        auxts_simd_interleave_s16(samples[0], samples[1], interleaved_samples, m);
        auxts_memory_stream_write(&pcm->memory_stream, interleaved_samples, m);

        free(interleaved_samples);
        return;
    }
}

