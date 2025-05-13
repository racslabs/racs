#include "pcm.h"

void racs_pcm_set_channels(racs_pcm *pcm, racs_uint16 channels) {
    pcm->channels = channels;
}

void racs_pcm_set_bit_depth(racs_pcm *pcm, racs_uint16 bit_depth) {
    pcm->bit_depth = bit_depth;
}

void racs_pcm_set_sample_rate(racs_pcm *pcm, racs_uint32 sample_rate) {
    pcm->sample_rate = sample_rate;
}

void racs_pcm_init(racs_pcm *pcm) {
    pcm->samples = 0;
    racs_memstream_init(&pcm->out_stream);
}

size_t racs_pcm_write_s16(racs_pcm *pcm, const racs_int16 *in, size_t samples) {
    return racs_pcm_write(pcm, in, samples);
}

size_t racs_pcm_write(racs_pcm *pcm, const void *in, size_t samples) {
    size_t size = pcm->channels * samples * pcm->bit_depth / 8;
    racs_memstream_write(&pcm->out_stream, in, size);
    pcm->samples += samples;

    return pcm->out_stream.current_pos;
}

void racs_pcm_destroy(racs_pcm *pcm) {
    free(pcm->out_stream.data);
}
