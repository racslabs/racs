
#ifndef RATS_SIMD_H
#define RATS_SIMD_H

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
#define RATS_DO_PRAGMA(x) _Pragma(#x)
#define RATS_UNROLL(N) RATS_DO_PRAGMA(GCC unroll N)
#endif

#endif //RATS_SIMD_H
