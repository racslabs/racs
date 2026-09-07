
#if defined(__AVX2__) || defined(__SSE4_1__)
    #include <immintrin.h>
#else
    #error "racs: simd error: neither __AVX2__ nor __SSE4_1__ is defined."
#endif   

#if defined(__GNUC__)
    #define RACS_UNROLL _Pragma("GCC unroll 4")
#else
    #define RACS_UNROLL
#endif


#include "../types.h"


void racs_simd_int16_int32(const racs_int16 *src, racs_int32 *dst, size_t n);

void racs_simd_int24_int32(const racs_int24 *src, racs_int32 *dst, size_t n);

void racs_simd_int32_int16(const racs_int32 *src, racs_int16 *dst, size_t n);

void racs_simd_int32_int24(const racs_int32 *src, racs_int24 *dst, size_t n);

void racs_simd_interleave_int16(const racs_int16 *src_l, const racs_int16 *src_r, racs_int16 *dst, size_t n);

void racs_simd_interleave_int24(const racs_int24 *src_l, const racs_int24 *src_r, racs_int24 *dst, size_t n);
