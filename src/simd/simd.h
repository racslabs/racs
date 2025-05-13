
#ifndef RACS_SIMD_H
#define RACS_SIMD_H

#include <stddef.h>
#include "../types.h"
#include "../export.h"
#include "pcm_simd.h"

#if defined(__AVX__) || defined(__SSE4_1__)
#include <immintrin.h>
#elif defined( __ARM_NEON__)

#include <arm_neon.h>

#else
#warning "SIMD instructions not found."
#endif

#if defined(__GNUG__) || defined(__GNUC__)
#define RACS_DO_PRAGMA(x) _Pragma(#x)
#define RACS_UNROLL(N) RACS_DO_PRAGMA(GCC unroll N)
#endif

#endif //RACS_SIMD_H
