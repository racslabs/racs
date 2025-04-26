#ifndef RATS_TYPES_H
#define RATS_TYPES_H

#include <time.h>

// Basic integer types
typedef signed char rats_int8;
typedef unsigned char rats_uint8;

typedef signed short rats_int16;
typedef unsigned short rats_uint16;

typedef signed int rats_int32;
typedef unsigned int rats_uint32;

typedef signed long long rats_int64;
typedef unsigned long long rats_uint64;

// 24-bit integer types (useful for file I/O and packing)
typedef struct {
    unsigned char _[3];
} rats_uint24;
typedef struct {
    signed char _[3];
} rats_int24;

// Standard time type
typedef time_t rats_time;

// Complex type (C99 and later)
#if __STDC_VERSION__ >= 199901L

#include <complex.h>

typedef float complex rats_complex;
#endif

#endif // RATS_TYPES_H
