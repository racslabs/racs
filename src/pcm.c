#include "pcm.h"

void auxts_pcm_init_s32(auxts_pcm* pcm, auxts_int32* buf, auxts_uint8 channels, auxts_uint32 sample_rate, size_t sample_count) {
    memset(pcm, 0, sizeof(auxts_pcm));

    pcm->channels = channels;
    pcm->sample_rate = sample_rate;
    pcm->bit_depth = AUXTS_PCM_BIT_DEPTH_24;

    auxts_memory_stream_init(&pcm->memory_stream, buf, channels * sample_count * sizeof(auxts_int32));
}

void auxts_pcm_init_s16(auxts_pcm* pcm, auxts_int16* buf, auxts_uint8 channels, auxts_uint32 sample_rate, size_t sample_count) {
    memset(pcm, 0, sizeof(auxts_pcm));

    pcm->channels = channels;
    pcm->sample_rate = sample_rate;
    pcm->bit_depth = AUXTS_PCM_BIT_DEPTH_16;

    auxts_memory_stream_init(&pcm->memory_stream, buf, channels * sample_count * sizeof(auxts_int16));
}

void auxts_pcm_write_s32i(auxts_pcm* pcm, const auxts_int32* samples, size_t sample_count) {
    auxts_memory_stream_write(&pcm->memory_stream, samples, pcm->channels * sample_count * sizeof(auxts_int32));
}

void auxts_pcm_write_s16i(auxts_pcm* pcm, const auxts_int16* samples, size_t sample_count) {
    auxts_memory_stream_write(&pcm->memory_stream, samples, pcm->channels * sample_count * sizeof(auxts_int16));
}

void auxts_pcm_write_s32p(auxts_pcm* pcm, const auxts_int32** samples, size_t sample_count) {
    size_t size = pcm->channels * sample_count * sizeof(auxts_int32);

    auxts_int32* interleaved_samples = malloc(size);

    auxts_simd_interleave_s32(samples[0], samples[1], interleaved_samples, size/pcm->channels);
    auxts_memory_stream_write(&pcm->memory_stream, interleaved_samples, size);

    free(interleaved_samples);
}

void auxts_pcm_write_s16p(auxts_pcm* pcm, const auxts_int16** samples, size_t sample_count) {
    size_t size = pcm->channels * sample_count * sizeof(auxts_int16);

    auxts_int16* interleaved_samples = malloc(size);

    auxts_simd_interleave_s16(samples[0], samples[1], interleaved_samples, size/pcm->channels);
    auxts_memory_stream_write(&pcm->memory_stream, interleaved_samples, size);

    free(interleaved_samples);
}

