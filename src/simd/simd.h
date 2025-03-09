
#ifndef AUXTS_SIMD_H
#define AUXTS_SIMD_H

#include <stddef.h>
#include <stdint.h>

#if defined(__AVX__) || defined(__SSE4_1__)
#include <immintrin.h>
#elif defined( __ARM_NEON__)
#include <arm_neon.h>
#else
#warning "SIMD instructions not found. Code will not compile."
#endif

#endif //AUXTS_SIMD_H
