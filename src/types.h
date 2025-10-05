#ifndef RACS_TYPES_H
#define RACS_TYPES_H

#include <time.h>
#include <stdlib.h>
#include "export.h"
#include "capi/allocator.h"

// Basic integer types
typedef signed char racs_int8;
typedef unsigned char racs_uint8;

typedef signed short racs_int16;
typedef unsigned short racs_uint16;

typedef signed int racs_int32;
typedef unsigned int racs_uint32;

typedef signed long long racs_int64;
typedef unsigned long long racs_uint64;

// 24-bit integer types (useful for file I/O and packing)
typedef struct __attribute__((packed)) {
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

#endif // RACS_TYPES_H
