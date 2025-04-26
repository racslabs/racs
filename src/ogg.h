
#ifndef RATS_OGG_H
#define RATS_OGG_H

#include <opus/opusenc.h>
#include "memory.h"

#define RATS_OGG_FRAMESIZE_20MS 0.02

typedef struct {
    rats_uint16 channels;
    rats_uint32 sample_rate;
} rats_ogg_format;

typedef struct {
    int err;
    rats_ogg_format format;
    OggOpusComments *comments;
    OggOpusEnc *enc;
    void *ptr;
    off_t r_pos;
    off_t w_pos;
} rats_ogg;

void rats_ogg_set_channels(rats_ogg *ogg, rats_uint16 channels);

void rats_ogg_set_sample_rate(rats_ogg *ogg, rats_uint32 sample_rate);

void rats_ogg_init(rats_ogg *ogg, void *out);

int rats_ogg_write_callback(void *user_data, const rats_uint8 *ptr, rats_int32 len);

int rats_ogg_read_callback(void *user_data);

size_t rats_ogg_write(rats_ogg *ogg, const void *in, void *out, size_t samples, size_t size);

size_t rats_ogg_write_s16(rats_ogg *ogg, const rats_int16 *in, void *out, size_t samples, size_t size);

void rats_ogg_destroy(rats_ogg *ogg);

#endif //RATS_OGG_H
