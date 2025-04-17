#include "pcm.h"

void auxts_pcm_set_channels(auxts_pcm* pcm, auxts_uint16 channels) {
    pcm->channels = channels;
}

void auxts_pcm_set_bit_depth(auxts_pcm* pcm, auxts_uint16 bit_depth) {
    pcm->bit_depth = bit_depth;
}

void auxts_pcm_set_sample_rate(auxts_pcm* pcm, auxts_uint32 sample_rate) {
    pcm->sample_rate = sample_rate;
}

void auxts_pcm_init(auxts_pcm* pcm) {
    pcm->samples = 0;
    auxts_memory_stream_init(&pcm->out_stream);
}

size_t auxts_pcm_write_s16(auxts_pcm* pcm, const auxts_int16* in, size_t samples) {
    return auxts_pcm_write(pcm, in, samples);
}

size_t auxts_pcm_write(auxts_pcm* pcm, const void* in, size_t samples) {
    size_t size = pcm->channels * samples * pcm->bit_depth/8;
    auxts_memory_stream_write(&pcm->out_stream, in, size);
    pcm->samples += samples;

    return pcm->out_stream.current_pos;
}

void auxts_pcm_destroy(auxts_pcm* pcm) {
    free(pcm->out_stream.data);
}
