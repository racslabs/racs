// RACS - Remote Audio Caching Server
// Copyright (c) 2025 RACS Labs, LLC. All rights reserved.
//
// Licensed under the RACS Source Available License (RACS-SAL-1.0).
// Non-commercial use only. Commercial use requires a paid license.
// Contact: sales@racslabs.com
//
// SPDX-License-Identifier: RACS-SAL-1.0

#ifndef RACS_ENCODE_H
#define RACS_ENCODE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "mp3.h"
#include "wav.h"
#include "ogg.h"

typedef struct {
    racs_uint16 channels;
    racs_uint32 sample_rate;
    racs_uint16 bit_depth;
} racs_encode;

size_t racs_encode_pcm(racs_encode *encode, const void *in, void *out, size_t samples, size_t size, const char *mime_type);

#ifdef __cplusplus
}
#endif

#endif //RACS_ENCODE_H
