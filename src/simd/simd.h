
#if defined(__AVX2__) || defined(__SSE4_1__)
    #include <immintrin.h>
#else
    #error "racs: simd error: neither __AVX2__ nor __SSE4_1__ is defined."
#endif    

#include "../types.h"


void racs_simd_int16_int32(const racs_int16 *src, racs_int32 *dst, size_t n);

void racs_simd_int24_int32(const racs_int24 *src, racs_int32 *dst, size_t n);
