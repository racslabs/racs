
#ifndef RATS_MP3_H
#define RATS_MP3_H

#include <lame/lame.h>
#include "types.h"

// This file is a wrapper of _lame

typedef struct {
    rats_uint16 channels;
    rats_uint32 sample_rate;
} rats_mp3_format;

typedef struct {
    rats_mp3_format    format;
    lame_t              _lame;
    off_t               pos;
} rats_mp3;

void rats_mp3_set_channels(rats_mp3* mp3, rats_uint16 channels);
void rats_mp3_set_sample_rate(rats_mp3* mp3, rats_uint32 sample_rate);
void rats_mp3_init(rats_mp3* mp3);

size_t rats_mp3_write(rats_mp3* mp3, const void* in, void* out, size_t samples, size_t size);
size_t rats_mp3_write_s16(rats_mp3* mp3, const rats_int16* in, void* out, size_t samples, size_t size);

void rats_mp3_destroy(rats_mp3* mp3);

#endif //RATS_MP3_H
