#include "pcm.h"

void auxts_pcm_init(auxts_pcm* pcm, auxts_uint8 channels, auxts_uint8 bit_depth, auxts_uint32 sample_rate) {
    memset(pcm, 0, sizeof(auxts_pcm));

    pcm->channels = channels;
    pcm->sample_rate = sample_rate;
    pcm->bit_depth = bit_depth;

    size_t size = 1024 * channels * pcm->bit_depth/8;

    void* data = malloc(size);
    if (!data) {
        perror("Failed to allocate data to auxts_pcm");
        return;
    }

    auxts_memory_stream_init(&pcm->memory_stream, data, size);
}


size_t auxts_pcm_write_s16(auxts_pcm* pcm, const auxts_int16* in, size_t samples) {
    return auxts_pcm_write(pcm, in, samples);
}

size_t auxts_pcm_write(auxts_pcm* pcm, const void* in, size_t samples) {
    size_t size = pcm->channels * samples * pcm->bit_depth/8;
    auxts_memory_stream_write(&pcm->memory_stream, in, size);
    pcm->samples += samples;

    return pcm->memory_stream.current_pos;
}

void auxts_pcm_destroy(auxts_pcm* pcm) {
    free(pcm->memory_stream.data);
}
