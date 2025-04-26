#include "pcm.h"

void rats_pcm_set_channels(rats_pcm *pcm, rats_uint16 channels) {
    pcm->channels = channels;
}

void rats_pcm_set_bit_depth(rats_pcm *pcm, rats_uint16 bit_depth) {
    pcm->bit_depth = bit_depth;
}

void rats_pcm_set_sample_rate(rats_pcm *pcm, rats_uint32 sample_rate) {
    pcm->sample_rate = sample_rate;
}

void rats_pcm_init(rats_pcm *pcm) {
    pcm->samples = 0;
    rats_memstream_init(&pcm->out_stream);
}

size_t rats_pcm_write_s16(rats_pcm *pcm, const rats_int16 *in, size_t samples) {
    return rats_pcm_write(pcm, in, samples);
}

size_t rats_pcm_write(rats_pcm *pcm, const void *in, size_t samples) {
    size_t size = pcm->channels * samples * pcm->bit_depth / 8;
    rats_memstream_write(&pcm->out_stream, in, size);
    pcm->samples += samples;

    return pcm->out_stream.current_pos;
}

void rats_pcm_destroy(rats_pcm *pcm) {
    free(pcm->out_stream.data);
}
