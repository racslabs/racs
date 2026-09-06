
#include "simd.h"


#if defined(__AVX2__)

void racs_simd_int16_int32(const racs_int16 *src, racs_int32 *dst, size_t n) {
    size_t i = 0, m = n & ~7;

    for ( ; i < m; i += 8) {
        __m128i _src = _mm_loadu_si128((const __m128i *) &src[i]);
        __m256i _dst = _mm256_cvtepi16_epi32(_src);

        _mm256_storeu_si256((__m256i *) &dst[i], _dst);
    }   
    
    for ( ; i < n; i++) {
        dst[i] = (racs_int32) src[i];
    }
}

void racs_simd_int24_int32(const racs_int24 *src, racs_int32 *dst, size_t n) {
    size_t i = 0, m = n & ~7;

    const racs_uint8 *_src = (const racs_uint8 *)src;

    __m128i _mask = _mm_setr_epi8(0, 1, 2, -1, 3, 4, 5, -1, 6, 7, 8, -1, 9, 10, 11, -1);

    for ( ; i < m; i += 8) {
        __m128i _b1 = _mm_loadu_si128((const __m128i *) &_src[i * 3]);
        __m128i _s1 = _mm_srai_epi32(_mm_slli_epi32(_mm_shuffle_epi8(_b1, _mask), 8), 8);

        _mm_storeu_si128((__m128i *) &dst[i], _s1);

        __m128i _b2 = _mm_loadu_si128((const __m128i *) &_src[(i + 4) * 3]);
        __m128i _s2 = _mm_srai_epi32(_mm_slli_epi32(_mm_shuffle_epi8(_b2, _mask), 8), 8);

        _mm_storeu_si128((__m128i *) &dst[i + 4], _s2);
    }

    for ( ; i < n; i++) {
        const racs_uint8 *b = (const racs_uint8 *) &src[i];

        racs_int32 val = (b[0]) | (b[1] << 8) | (b[2] << 16);
        dst[i] = (val & 0x800000) ? (val | ~0xffffff) : val;
    }
}

#endif 
