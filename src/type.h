
#ifndef AUXTS_TYPE_H
#define AUXTS_TYPE_H

typedef   signed char       auxts_int8;
typedef unsigned char       auxts_uint8;
typedef   signed short      auxts_int16;
typedef unsigned short      auxts_uint16;
typedef   signed int        auxts_int32;
typedef unsigned int        auxts_uint32;

typedef   signed long long  auxts_int64;
typedef unsigned long long  auxts_uint64;

#if __STDC_VERSION__ >= 199901L

#include <complex.h>
typedef float complex auxts_complex_t;

#endif

#endif //AUXTS_TYPE_H
