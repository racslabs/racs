// RACS - Remote Audio Caching Server
// Copyright (c) 2025 RACS Labs, LLC. All rights reserved.
//
// Licensed under the RACS Source Available License (RACS-SAL-1.0).
// Non-commercial use only. Commercial use requires a paid license.
// Contact: sales@racslabs.com
//
// SPDX-License-Identifier: RACS-SAL-1.0

#ifndef RACS_PCM_H
#define RACS_PCM_H

#ifdef __cplusplus
extern "C" {
#endif

#include "memory.h"

typedef struct {
    racs_uint8 channels;
    racs_uint8 bit_depth;
    racs_uint32 sample_rate;
    size_t samples;
    racs_memstream out_stream;
} racs_pcm;

void racs_pcm_set_channels(racs_pcm *pcm, racs_uint16 channels);

void racs_pcm_set_bit_depth(racs_pcm *pcm, racs_uint16 bit_depth);

void racs_pcm_set_sample_rate(racs_pcm *pcm, racs_uint32 sample_rate);

void racs_pcm_init(racs_pcm *pcm);

size_t racs_pcm_write_s16(racs_pcm *pcm, const racs_int16 *in, size_t samples);

size_t racs_pcm_write(racs_pcm *pcm, const void *in, size_t samples);

void racs_pcm_destroy(racs_pcm *pcm);

#ifdef __cplusplus
}
#endif

#endif //RACS_PCM_H
