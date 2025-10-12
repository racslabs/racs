// RACS - Remote Audio Caching Server
// Copyright (c) 2025 RACS Labs, LLC. All rights reserved.
//
// Licensed under the RACS Source Available License (RSAL-1.0).
// Non-commercial use only. Commercial use requires a paid license.
// Contact: sales@racslabs.com
//
// SPDX-License-Identifier: RACS-SAL-1.0

#ifndef RACS_MURMUR3
#define RACS_MURMUR3

#include "types.h"
#include "export.h"

#ifdef __cplusplus
extern "C" {
#endif

#define RACS_ROTL64(x,y)	rotl64(x,y)

#define RACS_BIG_CONSTANT(x) (x##LLU)

#define racs_getblock(p, i) (p[i])

static RACS_FORCE_INLINE racs_uint64 rotl64(racs_uint64 x, int8_t r) {
    return (x << r) | (x >> (64 - r));
}

static RACS_FORCE_INLINE racs_uint64 fmix64(racs_uint64 k) {
    k ^= k >> 33;
    k *= RACS_BIG_CONSTANT(0xff51afd7ed558ccd);
    k ^= k >> 33;
    k *= RACS_BIG_CONSTANT(0xc4ceb9fe1a85ec53);
    k ^= k >> 33;

    return k;
    }

void murmur3_x64_128(const void *key, const int len, const racs_uint32 seed, void *out);

#ifdef __cplusplus
}
#endif

#endif // RACS_MURMUR3