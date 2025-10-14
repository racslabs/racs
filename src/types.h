// RACS - Remote Audio Caching Server
// Copyright (c) 2025 RACS Labs, LLC. All rights reserved.
//
// Licensed under the RACS Source Available License (RACS-SAL-1.0).
// Non-commercial use only. Commercial use requires a paid license.
// Contact: sales@racslabs.com
//
// SPDX-License-Identifier: RACS-SAL-1.0

#ifndef RACS_TYPES_H
#define RACS_TYPES_H

#ifdef __cplusplus
extern "C" {
#endif

#include <time.h>
#include <stdlib.h>
#include "export.h"

// Basic integer types
typedef signed char racs_int8;
typedef unsigned char racs_uint8;

typedef signed short racs_int16;
typedef unsigned short racs_uint16;

typedef signed int racs_int32;
typedef unsigned int racs_uint32;

typedef signed long long racs_int64;
typedef unsigned long long racs_uint64;

// 24-bit integer type (useful for file I/O and packing)
typedef struct RACS_PACKED {
    racs_uint8 _[3];
} racs_int24;

// Standard time type
typedef time_t racs_time;

// Complex type (C99 and later)
#if __STDC_VERSION__ >= 199901L

#include <complex.h>

typedef float complex racs_complex;
#endif

float *racs_s16_f32(const racs_int16 *in, size_t n);

racs_int16 *racs_f32_s16(const float *in, size_t n);

racs_int32 *racs_s16_s32(const racs_int16 *in, size_t n);

racs_int32 *racs_s24_s32(const racs_int24 *in, size_t n);

racs_int24 *racs_s32_s24(const racs_int32 *in, size_t n);

racs_int16 *racs_s32_s16(const racs_int32 *in, size_t n);

racs_int16 *racs_s32_s16_d8(const racs_int32 *in, size_t n);

#ifdef __cplusplus
}
#endif

#endif // RACS_TYPES_H
