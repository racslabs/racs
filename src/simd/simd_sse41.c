
#include "simd.h"


#if defined(__SSE4_1__) && !defined(__AVX2__)

void racs_simd_int16_int32(const racs_int16 *src, racs_int32 *dst, size_t n) {
    if (!src || !dst || n == 0) {
        return;
    }
    
    size_t i = 0, m = n & ~3;

    for ( ; i < m; i += 4) {
        __m128i _src = _mm_loadl_epi64((const __m128i *) &src[i]);
        __m128i _dst = _mm_cvtepi16_epi32(_src);

        _mm_storeu_si128((__m128i *) &dst[i], _dst);
    }   

    for (; i < n; i++) {
        dst[i] = (racs_int32) src[i];
    }
}

void racs_simd_int24_int32(const racs_int24 *src, racs_int32 *dst, size_t n) {
    if (!src || !dst || n == 0) {
        return;
    }
    
    size_t i = 0, m = n & ~3;

    const racs_uint8 *_src = (const racs_uint8 *) src;

    __m128i _mask = _mm_setr_epi8(0, 1, 2, -1, 3, 4, 5, -1, 6, 7, 8, -1, 9, 10, 11, -1);

    for ( ; i < m; i += 4) {
        __m128i _b1 = _mm_loadu_si128((const __m128i *) &_src[i * 3]);
        __m128i _s1 = _mm_srai_epi32(_mm_slli_epi32(_mm_shuffle_epi8(_b1, _mask), 8), 8);
        
        _mm_storeu_si128((__m128i *) &dst[i], _s1);
    }

    for (; i < n; i++) {
        const racs_uint8 *b = (const racs_uint8 *) &src[i];

        racs_int32 val = (b[0]) | (b[1] << 8) | (b[2] << 16);
        dst[i] = (val & 0x800000) ? (val | ~0xffffff) : val;
    }
}

void racs_simd_int32_int16(const racs_int32 *src, racs_int16 *dst, size_t n) {
    if (!src || !dst || n == 0) {
        return;
    }

    size_t i = 0, m = n & ~7;

    for ( ; i < m; i += 8) {
        __m128i _src1 = _mm_loadu_si128((const __m128i *) &src[i]);
        __m128i _src2 = _mm_loadu_si128((const __m128i *) &src[i + 4]);

        __m128i _dst = _mm_packs_epi32(_src1, _src2);
        _mm_storeu_si128((__m128i *) &dst[i], _dst);
    }

    for (; i < n; i++) {
        racs_int32 val = src[i];

        if (val > 32767) {
            val = 32767;
        } else if (val < -32768) {
            val = -32768;
        }
    
        dst[i] = (racs_int16) val;
    }
}

void racs_simd_int32_int24(const racs_int32 *src, racs_int24 *dst, size_t n) {
    if (!src || !dst || n == 0) {
        return;
    }

    size_t i = 0, m = n & ~3;
    racs_uint8 *_dst = (racs_uint8 *)dst;

    __m128i _mask = _mm_setr_epi8(0, 1, 2, 4, 5, 6, 8, 9, 10, 12, 13, 14, -1, -1, -1, -1);

    for ( ; i < m; i += 4) {
        __m128i _src = _mm_loadu_si128((const __m128i *) &src[i]);
        __m128i _packed = _mm_shuffle_epi8(_src, _mask);

        _mm_storeu_si64(&_dst[i * 3], _packed);
        _mm_storeu_si32(&_dst[(i * 3) + 8], _mm_srli_si128(_packed, 8));
    }

    for ( ; i < n; i++) {
        racs_int32 val = src[i];
        
        if (val > 8388607) {
            val = 8388607;
        } else if (val < -8388608) {
            val = -8388608;
        }

        racs_uint8 *b = (racs_uint8 *) &dst[i];

        b[0] = (racs_uint8)(val & 0xff);
        b[1] = (racs_uint8)((val >> 8) & 0xff);
        b[2] = (racs_uint8)((val >> 16) & 0xff);
    }
}

void racs_simd_interleave_int16(const racs_int16 *src_l, const racs_int16 *src_r, racs_int16 *dst, size_t n) {
    if (!src_l || !src_r || !dst || n == 0) {
        return;
    }

    size_t i = 0, m = n & ~7; 

    for ( ; i < m; i += 8) {
        __m128i _src_l = _mm_loadu_si128((const __m128i *) &src_l[i]);
        __m128i _src_r = _mm_loadu_si128((const __m128i *) &src_r[i]);

        __m128i _dst_l = _mm_unpacklo_epi16(_src_l, _src_r);
        __m128i _dst_r = _mm_unpackhi_epi16(_src_l, _src_r);

        _mm_storeu_si128((__m128i *) &dst[i * 2], _dst_l);
        _mm_storeu_si128((__m128i *) &dst[(i * 2) + 8], _dst_r);
    }

    for ( ; i < n; i++) {
        dst[i * 2]       = src_l[i];
        dst[(i * 2) + 1] = src_r[i];
    }
}

#endif 
