// RACS - Remote Audio Caching Server
// Copyright (c) 2025 RACS Labs, LLC. All rights reserved.
//
// Licensed under the RACS Source Available License (RACS-SAL-1.0).
// Non-commercial use only. Commercial use requires a paid license.
// Contact: sales@racslabs.com
//
// SPDX-License-Identifier: RACS-SAL-1.0

#ifndef RACS_MP3_H
#define RACS_MP3_H

#include <lame/lame.h>
#include "types.h"

// This file is a wrapper of lame

typedef struct {
    racs_uint16 channels;
    racs_uint32 sample_rate;
} racs_mp3_format;

typedef struct {
    racs_mp3_format format;
    lame_t _lame;
    off_t pos;
} racs_mp3;

void racs_mp3_set_channels(racs_mp3 *mp3, racs_uint16 channels);

void racs_mp3_set_sample_rate(racs_mp3 *mp3, racs_uint32 sample_rate);

void racs_mp3_init(racs_mp3 *mp3);

size_t racs_mp3_write(racs_mp3 *mp3, const void *in, void *out, size_t samples, size_t size);

size_t racs_mp3_write_s16(racs_mp3 *mp3, const racs_int16 *in, void *out, size_t samples, size_t size);

void racs_mp3_destroy(racs_mp3 *mp3);

#endif //RACS_MP3_H
