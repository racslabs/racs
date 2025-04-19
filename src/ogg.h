
#ifndef AUXTS_OGG_H
#define AUXTS_OGG_H

#include <opus/opusenc.h>
#include "memory.h"

#define AUXTS_OGG_FRAMESIZE_20MS 0.02

typedef struct {
    auxts_uint16 channels;
    auxts_uint32 sample_rate;
} auxts_ogg_format;

typedef struct {
    int                 err;
    auxts_ogg_format    format;
    OggOpusComments*    comments;
    OggOpusEnc*         enc;
    void*               ptr;
    off_t               r_pos;
    off_t               w_pos;
} auxts_ogg;

void auxts_ogg_set_channels(auxts_ogg* ogg, auxts_uint16 channels);
void auxts_ogg_set_sample_rate(auxts_ogg* ogg, auxts_uint32 sample_rate);

void auxts_ogg_init(auxts_ogg* ogg, void* out);

int auxts_ogg_write_callback(void *user_data, const auxts_uint8* ptr, auxts_int32 len);
int auxts_ogg_read_callback(void *user_data);

size_t auxts_ogg_write(auxts_ogg* ogg, const void* in, void* out, size_t samples, size_t size);
size_t auxts_ogg_write_s16(auxts_ogg* ogg, const auxts_int16* in, void* out, size_t samples, size_t size);

void auxts_ogg_destroy(auxts_ogg* ogg);

#endif //AUXTS_OGG_H
