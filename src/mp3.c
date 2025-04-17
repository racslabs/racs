#include "mp3.h"

void auxts_mp3_set_channels(auxts_mp3* mp3, auxts_uint16 channels) {
    mp3->format.channels = channels;
}

void auxts_mp3_set_sample_rate(auxts_mp3* mp3, auxts_uint32 sample_rate) {
    mp3->format.sample_rate = sample_rate;
}

void auxts_mp3_init(auxts_mp3* mp3) {
    mp3->_lame = lame_init();

    lame_set_in_samplerate(mp3->_lame, (int)mp3->format.sample_rate);
    lame_set_num_channels(mp3->_lame, mp3->format.channels);
    lame_set_VBR(mp3->_lame, vbr_default);
    lame_init_params(mp3->_lame);
}

size_t auxts_mp3_write(auxts_mp3* mp3, const void* in, void* out, size_t samples, size_t size) {
    mp3->pos = lame_encode_buffer_interleaved(mp3->_lame, (auxts_int16*)in, (int)samples, out, (int)size);
    return mp3->pos;
}

size_t auxts_mp3_write_s16(auxts_mp3* mp3, const auxts_int16* in, void* out, size_t samples, size_t size) {
    return auxts_mp3_write(mp3, in, out, samples, size);
}

void auxts_mp3_destroy(auxts_mp3* mp3) {
    lame_close(mp3->_lame);
}
