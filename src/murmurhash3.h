// RACS - Remote Audio Caching Server
// Copyright (c) 2025 RACS Labs, LLC. All rights reserved.
//
// Licensed under the RACS Source Available License (RACS-SAL-1.0).
// Non-commercial use only. Commercial use requires a paid license.
// Contact: sales@racslabs.com
//

#ifndef RACS_MURMURHASH3_H
#define RACS_MURMURHASH3_H

#ifdef __cplusplus
extern "C" {
#endif

#include "types.h"

#define RACS_ROTL64(x,y)	 racs_rotl64(x,y)

#define RACS_BIG_CONSTANT(x) (x##LLU)

#define RACS_GETBLOCK(p, i)  (p[i])

static __attribute__((always_inline)) racs_uint64 racs_rotl64(racs_uint64 x, racs_int8 r) {
    return (x << r) | (x >> (64 - r));
}

static __attribute__((always_inline)) racs_uint64 racs_fmix64(racs_uint64 k) {
    k ^= k >> 33;
    k *= RACS_BIG_CONSTANT(0xff51afd7ed558ccd);
    k ^= k >> 33;
    k *= RACS_BIG_CONSTANT(0xc4ceb9fe1a85ec53);
    k ^= k >> 33;

    return k;
}

void racs_murmurhash3_x64_128(const void *key, int len, racs_uint32 seed, void *out);

#ifdef __cplusplus
}
#endif

#endif //RACS_MURMURHASH3_H
