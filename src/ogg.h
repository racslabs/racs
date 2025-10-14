// RACS - Remote Audio Caching Server
// Copyright (c) 2025 RACS Labs, LLC. All rights reserved.
//
// Licensed under the RACS Source Available License (RACS-SAL-1.0).
// Non-commercial use only. Commercial use requires a paid license.
// Contact: sales@racslabs.com
//
// SPDX-License-Identifier: RACS-SAL-1.0

#ifndef RACS_OGG_H
#define RACS_OGG_H

#include <opus/opusenc.h>
#include "memory.h"
#include "log.h"

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

size_t racs_ogg_write(racs_ogg *ogg, const void *in, void *out, size_t samples);

size_t racs_ogg_write_s16(racs_ogg *ogg, const racs_int16 *in, void *out, size_t samples);

void racs_ogg_destroy(racs_ogg *ogg);

#endif //RACS_OGG_H
