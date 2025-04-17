
#ifndef AUXTS_OGG_H
#define AUXTS_OGG_H

#include <opus/opusenc.h>
#include "stream.h"

typedef struct {
    auxts_uint16 channels;
    auxts_uint32 sample_rate;
} auxts_ogg_format;

typedef struct {
    int                 err;
    auxts_ogg_format    format;
    OggOpusComments*    comments;
    OggOpusEnc*         enc;
    off_t               r_pos;
    off_t               w_pos;
} auxts_ogg;

void auxts_ogg_set_channels(auxts_ogg* ogg, auxts_uint16 channels);
void auxts_ogg_set_sample_rate(auxts_ogg* ogg, auxts_uint32 sample_rate);

void auxts_ogg_init(auxts_ogg* ogg);

void auxts_ogg_read_pcm_frames(auxts_ogg* ogg, const auxts_int16* in, size_t samples);

size_t auxts_ogg_write(auxts_ogg* ogg, const void* in, void* out, size_t samples, size_t size);
size_t auxts_ogg_write_s16(auxts_ogg* ogg, const auxts_int16* in, void* out, size_t samples, size_t size);

#endif //AUXTS_OGG_H
