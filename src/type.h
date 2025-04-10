#ifndef AUXTS_TYPE_H
#define AUXTS_TYPE_H

#include <time.h>

// Basic integer types
typedef   signed char       auxts_int8;
typedef unsigned char       auxts_uint8;

typedef   signed short      auxts_int16;
typedef unsigned short      auxts_uint16;

typedef   signed int        auxts_int32;
typedef unsigned int        auxts_uint32;

typedef   signed long long  auxts_int64;
typedef unsigned long long  auxts_uint64;

// 24-bit integer types (useful for file I/O and packing)
typedef struct { unsigned char _[3]; } auxts_uint24;
typedef struct {   signed char _[3]; } auxts_int24;

// Standard time type
typedef time_t               auxts_time;

// Complex type (C99 and later)
#if __STDC_VERSION__ >= 199901L
#include <complex.h>
typedef float complex auxts_complex;
#endif

#endif // AUXTS_TYPE_H
