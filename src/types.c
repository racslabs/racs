// RACS - Remote Audio Caching Server
// Copyright (c) 2025 RACS Labs, LLC. All rights reserved.
//
// Licensed under the RACS Source Available License (RACS-SAL-1.0).
// Non-commercial use only. Commercial use requires a paid license.
// Contact: sales@racslabs.com
//
// SPDX-License-Identifier: RACS-SAL-1.0

#include "types.h"

float *racs_s16_f32(const racs_int16 *in, size_t n) {
    float *out = NULL;
    posix_memalign((void **)&out, RACS_ALIGN, sizeof(float) * n);

    for (int i = 0; i < n; ++i)
        out[i] = (float)in[i] / 32767;

    return out;
}

racs_int16 *racs_f32_s16(const float *in, size_t n) {
    racs_int16 *out = malloc(sizeof(racs_int16) * n);

    for (int i = 0; i < n; ++i)
        out[i] = (racs_int16)(in[i] * 32767);

    return out;
}

racs_int32 *racs_s16_s32(const racs_int16 *in, size_t n) {
    racs_int32 *out = malloc(sizeof(racs_int32) * n);

    for (int i = 0; i < n; ++i)
        out[i] = in[i];

    return out;
}

racs_int32 *racs_s24_s32(const racs_int24 *in, size_t n) {
    racs_int32 *out = malloc(sizeof(racs_int32) * n);

    for (int i = 0; i < n; ++i) {
        racs_int32 s = (in[i]._[0] | (in[i]._[1] << 8) | (in[i]._[2] << 16));
        if (s & 0x800000) s |= ~0xffffff;
        out[i] = s;
    }

    return out;
}

racs_int24 *racs_s32_s24(const racs_int32 *in, size_t n) {
    racs_int24 *out = malloc(sizeof(racs_int24) * n);

    for (int i = 0; i < n; ++i) {
        racs_int32 s = in[i];

        out[i]._[0] = (racs_uint8)(s & 0xff);
        out[i]._[1] = (racs_uint8)((s >> 8) & 0xff);
        out[i]._[2] = (racs_uint8)((s >> 16) & 0xff);
    }

    return out;
}

racs_int16 *racs_s32_s16(const racs_int32 *in, size_t n) {
    racs_int16 *out = malloc(sizeof(racs_int16) * n);

    for (int i = 0; i < n; ++i)
        out[i] = (racs_int16)in[i];

    return out;
}

racs_int16 *racs_s32_s16_d8(const racs_int32 *in, size_t n) {
    racs_int16 *out = malloc(sizeof(racs_int16) * n);

    for (int i = 0; i < n; ++i)
        out[i] = (racs_int16)(in[i] >> 8);

    return out;
}