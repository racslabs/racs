// RACS - Remote Audio Caching Server
// Copyright (c) 2025 RACS Labs, LLC. All rights reserved.
//
// Licensed under the RACS Source Available License (RSAL-1.0).
// Non-commercial use only. Commercial use requires a paid license.
// Contact: sales@racslabs.com
//
// SPDX-License-Identifier: RACS-SAL-1.0

#include "murmur3.h"

void murmur3_x64_128(const void * key, const int len, const racs_uint32 seed, void * out) {
    const racs_uint8 * data = (const racs_uint8*)key;
    const int n = len / 16;

    int i;

    racs_uint64 h1 = seed;
    racs_uint64 h2 = seed;

    racs_uint64 c1 = RACS_BIG_CONSTANT(0x87c37b91114253d5);
    racs_uint64 c2 = RACS_BIG_CONSTANT(0x4cf5ad432745937f);

    const racs_uint64 * blocks = (const racs_uint64 *)(data);

    for(i = 0; i < n; i++)
    {
        racs_uint64 k1 = racs_getblock(blocks, i * 2 + 0);
        racs_uint64 k2 = racs_getblock(blocks, i * 2 + 1);

        k1 *= c1;
        k1  = RACS_ROTL64(k1, 31);
        k1 *= c2;
        h1 ^= k1;

        h1 = RACS_ROTL64(h1, 27);
        h1 += h2;
        h1 = h1 * 5 + 0x52dce729;

        k2 *= c2;
        k2  = RACS_ROTL64(k2, 33);
        k2 *= c1;
        h2 ^= k2;

        h2 = RACS_ROTL64(h2, 31);
        h2 += h1;
        h2 = h2 * 5 + 0x38495ab5;
    }

    const racs_uint8 * tail = (const racs_uint8*)(data + n * 16);

    racs_uint64 k1 = 0;
    racs_uint64 k2 = 0;

    switch(len & 15) {
        case 15: k2 ^= (racs_uint64)(tail[14]) << 48;
        case 14: k2 ^= (racs_uint64)(tail[13]) << 40;
        case 13: k2 ^= (racs_uint64)(tail[12]) << 32;
        case 12: k2 ^= (racs_uint64)(tail[11]) << 24;
        case 11: k2 ^= (racs_uint64)(tail[10]) << 16;
        case 10: k2 ^= (racs_uint64)(tail[ 9]) << 8;
        case  9: k2 ^= (racs_uint64)(tail[ 8]) << 0;

            k2 *= c2;
            k2  = RACS_ROTL64(k2, 33);
            k2 *= c1;
            h2 ^= k2;

        case  8: k1 ^= (racs_uint64)(tail[ 7]) << 56;
        case  7: k1 ^= (racs_uint64)(tail[ 6]) << 48;
        case  6: k1 ^= (racs_uint64)(tail[ 5]) << 40;
        case  5: k1 ^= (racs_uint64)(tail[ 4]) << 32;
        case  4: k1 ^= (racs_uint64)(tail[ 3]) << 24;
        case  3: k1 ^= (racs_uint64)(tail[ 2]) << 16;
        case  2: k1 ^= (racs_uint64)(tail[ 1]) << 8;
        case  1: k1 ^= (racs_uint64)(tail[ 0]) << 0;

            k1 *= c1;
            k1  = RACS_ROTL64(k1, 31);
            k1 *= c2;
            h1 ^= k1;
    }

    h1 ^= len; h2 ^= len;

    h1 += h2;
    h2 += h1;

    h1 = fmix64(h1);
    h2 = fmix64(h2);

    h1 += h2;
    h2 += h1;

    ((racs_uint64*)out)[0] = h1;
    ((racs_uint64*)out)[1] = h2;
}
