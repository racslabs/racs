
#ifndef AUXTS_MP3_H
#define AUXTS_MP3_H

#include <lame/lame.h>
#include "types.h"

// This file is a wrapper of _lame

typedef struct {
    auxts_uint16 channels;
    auxts_uint32 sample_rate;
} auxts_mp3_format;

typedef struct {
    auxts_mp3_format    format;
    lame_t              _lame;
    off_t               pos;
} auxts_mp3;

void auxts_mp3_set_channels(auxts_mp3* mp3, auxts_uint16 channels);
void auxts_mp3_set_sample_rate(auxts_mp3* mp3, auxts_uint32 sample_rate);
void auxts_mp3_init(auxts_mp3* mp3);

size_t auxts_mp3_write(auxts_mp3* mp3, const void* in, void* out, size_t samples, size_t size);
size_t auxts_mp3_write_s16(auxts_mp3* mp3, const auxts_int16* in, void* out, size_t samples, size_t size);

void auxts_mp3_destroy(auxts_mp3* mp3);

#endif //AUXTS_MP3_H
