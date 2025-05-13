
#ifndef RACS_OGG_H
#define RACS_OGG_H

#include <opus/opusenc.h>
#include "memory.h"

#define RACS_OGG_FRAMESIZE_20MS 0.02

typedef struct {
    racs_uint16 channels;
    racs_uint32 sample_rate;
} racs_ogg_format;

typedef struct {
    int err;
    racs_ogg_format format;
    OggOpusComments *comments;
    OggOpusEnc *enc;
    void *ptr;
    off_t r_pos;
    off_t w_pos;
} racs_ogg;

void racs_ogg_set_channels(racs_ogg *ogg, racs_uint16 channels);

void racs_ogg_set_sample_rate(racs_ogg *ogg, racs_uint32 sample_rate);

void racs_ogg_init(racs_ogg *ogg, void *out);

int racs_ogg_write_callback(void *user_data, const racs_uint8 *ptr, racs_int32 len);

int racs_ogg_read_callback(void *user_data);

size_t racs_ogg_write(racs_ogg *ogg, const void *in, void *out, size_t samples, size_t size);

size_t racs_ogg_write_s16(racs_ogg *ogg, const racs_int16 *in, void *out, size_t samples, size_t size);

void racs_ogg_destroy(racs_ogg *ogg);

#endif //RACS_OGG_H
