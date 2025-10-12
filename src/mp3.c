// RACS - Remote Audio Caching Server
// Copyright (c) 2025 RACS Labs, LLC. All rights reserved.
//
// Licensed under the RACS Source Available License (RSAL-1.0).
// Non-commercial use only. Commercial use requires a paid license.
// Contact: sales@racslabs.com
//
// SPDX-License-Identifier: RACS-SAL-1.0

#include "mp3.h"

void racs_mp3_set_channels(racs_mp3 *mp3, racs_uint16 channels) {
    mp3->format.channels = channels;
}

void racs_mp3_set_sample_rate(racs_mp3 *mp3, racs_uint32 sample_rate) {
    mp3->format.sample_rate = sample_rate;
}

void racs_mp3_init(racs_mp3 *mp3) {
    mp3->_lame = lame_init();
    mp3->pos = 0;

    lame_set_in_samplerate(mp3->_lame, (int) mp3->format.sample_rate);
    lame_set_num_channels(mp3->_lame, mp3->format.channels);
    lame_set_VBR(mp3->_lame, vbr_default);
    lame_init_params(mp3->_lame);
}

size_t racs_mp3_write(racs_mp3 *mp3, const void *in, void *out, size_t samples, size_t size) {
    if (mp3->format.channels == 1)
        mp3->pos = lame_encode_buffer(mp3->_lame, (racs_int16 *) in, NULL, (int) samples, out, (int) size);
    else if (mp3->format.channels == 2)
        mp3->pos = lame_encode_buffer_interleaved(mp3->_lame, (racs_int16 *) in, (int) samples, out, (int) size);
    return mp3->pos;
}

size_t racs_mp3_write_s16(racs_mp3 *mp3, const racs_int16 *in, void *out, size_t samples, size_t size) {
    return racs_mp3_write(mp3, in, out, samples, size);
}

void racs_mp3_destroy(racs_mp3 *mp3) {
    lame_close(mp3->_lame);
}
