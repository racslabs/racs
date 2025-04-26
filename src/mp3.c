#include "mp3.h"

void rats_mp3_set_channels(rats_mp3 *mp3, rats_uint16 channels) {
    mp3->format.channels = channels;
}

void rats_mp3_set_sample_rate(rats_mp3 *mp3, rats_uint32 sample_rate) {
    mp3->format.sample_rate = sample_rate;
}

void rats_mp3_init(rats_mp3 *mp3) {
    mp3->_lame = lame_init();

    lame_set_in_samplerate(mp3->_lame, (int) mp3->format.sample_rate);
    lame_set_num_channels(mp3->_lame, mp3->format.channels);
    lame_set_VBR(mp3->_lame, vbr_default);
    lame_init_params(mp3->_lame);
}

size_t rats_mp3_write(rats_mp3 *mp3, const void *in, void *out, size_t samples, size_t size) {
    mp3->pos = lame_encode_buffer_interleaved(mp3->_lame, (rats_int16 *) in, (int) samples, out, (int) size);
    return mp3->pos;
}

size_t rats_mp3_write_s16(rats_mp3 *mp3, const rats_int16 *in, void *out, size_t samples, size_t size) {
    return rats_mp3_write(mp3, in, out, samples, size);
}

void rats_mp3_destroy(rats_mp3 *mp3) {
    lame_close(mp3->_lame);
}
