
#include "simd.h"


#if defined(__AVX2__)

void racs_simd_int16_int32(const racs_int16 *src, racs_int32 *dst, size_t n) {
    if (!src || !dst || n == 0) {
        return;
    }

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
    if (!src || !dst || n == 0) {
        return;
    }
    

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

void racs_simd_int32_int16(const racs_int32 *src, racs_int16 *dst, size_t n) {
    if (!src || !dst || n == 0) {
        return;
    }

    size_t i = 0, m = n & ~15;

    for ( ; i < m; i += 16) {
        __m256i _src1 = _mm256_loadu_si256((const __m256i *) &src[i]);
        __m256i _src2 = _mm256_loadu_si256((const __m256i *) &src[i + 8]);

        __m256i _packed = _mm256_packs_epi32(_src1, _src2);
        __m256i _dst = _mm256_permute4x64_epi64(_packed, _MM_SHUFFLE(3, 1, 2, 0));

        _mm256_storeu_si256((__m256i *) &dst[i], _dst);
    }

    for ( ; i < n; i++) {
        racs_int32 val = src[i];

        if (val > 32767)  {
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

    size_t i = 0, m = n & ~7;
    racs_uint8 *_dst = (racs_uint8 *)dst;

    __m256i _mask = _mm256_setr_epi8(
        0, 1, 2, 4, 5, 6, 8, 9, 10, 12, 13, 14, -1, -1, -1, -1, 
        0, 1, 2, 4, 5, 6, 8, 9, 10, 12, 13, 14, -1, -1, -1, -1  
    );

    __m256i _permute_mask = _mm256_setr_epi32(0, 1, 2, 4, 5, 6, -1, -1);

    for ( ; i < m; i += 8) {
        __m256i _src = _mm256_loadu_si256((const __m256i *) &src[i]);
        
        __m256i _shuffled = _mm256_shuffle_epi8(_src, _mask);
        __m256i _ready = _mm256_permutevar8x32_epi32(_shuffled, _permute_mask);

        _mm_storeu_si128((__m128i *) &_dst[i * 3], _mm256_castsi256_si128(_ready));
        _mm_storeu_si64(&_dst[(i * 3) + 16], _mm256_extracti128_si256(_ready, 1));
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

    size_t i = 0, m = n & ~15;

    for ( ; i < m; i += 16) {
        __m256i _src_l = _mm256_loadu_si256((const __m256i *) &src_l[i]);
        __m256i _src_r = _mm256_loadu_si256((const __m256i *) &src_r[i]);

        __m256i _unp_lo = _mm256_unpacklo_epi16(_src_l, _src_r);
        __m256i _unp_hi = _mm256_unpackhi_epi16(_src_l, _src_r);

        __m256i _dst_l = _mm256_permute2x128_si256(_unp_lo, _unp_hi, _MM_SHUFFLE(0, 2, 0, 0));
        __m256i _dst_r = _mm256_permute2x128_si256(_unp_lo, _unp_hi, _MM_SHUFFLE(0, 3, 0, 1));

        _mm256_storeu_si256((__m256i *) &dst[i * 2], _dst_l);
        _mm256_storeu_si256((__m256i *) &dst[(i * 2) + 16], _dst_r);
    }

    for ( ; i < n; i++) {
        dst[i * 2]       = src_l[i];
        dst[(i * 2) + 1] = src_r[i];
    }
}


static inline __m256i racs_avx2_xorshift32(__m256i *state) {
    __m256i x = *state;

    x = _mm256_xor_si256(x, _mm256_slli_epi32(x, 13));
    x = _mm256_xor_si256(x, _mm256_srli_epi32(x, 17));
    x = _mm256_xor_si256(x, _mm256_slli_epi32(x, 5));

    *state = x;

    return x;
}

void racs_simd_dither_int24_int16(const racs_int24 *src, racs_int16 *dst, size_t n) {
    static __thread racs_uint32 _seed[8] = {
        0x12345678, 0x87654321, 0xabcdef01, 0x10fedcba,
        0x98765432, 0x23456789, 0xfedcba01, 0x01abcdef
    };

    size_t i = 0, m = n & ~7; 
    
    __m256i _state = _mm256_loadu_si256((const __m256i*)_seed);

    __m256i _mask = _mm256_setr_epi8(
        0, 1, 2, 2,    3, 4, 5, 5,    6, 7, 8, 8,    9, 10, 11, 11,
        12, 13, 14, 14, 15, 16, 17, 17, 18, 19, 20, 20, 21, 22, 23, 23
    );

    const racs_uint8 *_src = (const racs_uint8 *) src;

    for ( ; i < m; i += 8) {
        __m128i _l = _mm_loadu_si128((const __m128i *)(_src + i * 3));
        __m128i _h = _mm_loadl_epi64((const __m128i *)(_src + i * 3 + 16));

        __m256i _b = _mm256_castsi128_si256(_l);
        _b = _mm256_inserti128_si256(_b, _h, 1);

        __m256i _i = _mm256_shuffle_epi8(_b, _mask);
        
        _i = _mm256_slli_epi32(_i, 8);
        _i = _mm256_srai_epi32(_i, 8);

        __m256i _r1 = racs_avx2_xorshift32(&_state);
        __m256i _r2 = racs_avx2_xorshift32(&_state);

        _r1 = _mm256_srai_epi32(_mm256_slli_epi32(_r1, 24), 24);
        _r2 = _mm256_srai_epi32(_mm256_slli_epi32(_r2, 24), 24);

        __m256i _tpdf = _mm256_add_epi32(_r1, _r2);

        _i = _mm256_add_epi32(_i, _tpdf);
        _i = _mm256_srai_epi32(_i, 8);

        __m256i _dst = _mm256_packs_epi32(_i, _i);
        _dst = _mm256_permute4x64_epi64(_dst, _MM_SHUFFLE(3, 1, 2, 0));

        _mm_storeu_si128((__m128i*)(dst + i), _mm256_castsi256_si128(_dst));
    }

    _mm256_storeu_si256((__m256i *) _seed, _state);

    racs_uint32 s1 = _seed[0] ^ 0x55555555;
    racs_uint32 s2 = _seed[1] ^ 0xaaaaaaaa;

    for ( ; i < n; i++) {
        racs_int32 s = _src[i * 3] | (_src[i * 3 + 1] << 8) | (_src[i * 3 + 2] << 16);
        if (s & 0x800000) {
            s |= 0xff000000;
        }

        s1 ^= (s1 << 13); 
        s1 ^= (s1 >> 17); 
        s1 ^= (s1 << 5);

        s2 ^= (s2 << 13); 
        s2 ^= (s2 >> 17); 
        s2 ^= (s2 << 5);

        racs_int32 tpdf = ((racs_int32)(s1 << 24) >> 24) + ((racs_int32)(s2 << 24) >> 24);
        racs_int32 dithered = s + tpdf;

        dithered >>= 8;
        
        if (dithered > 32767) {
            dithered = 32767;
        } else if (dithered < -32768) {
            dithered = -32768;
        }
        
        dst[i] = (racs_int16) dithered;
    }
}

void racs_simd_int16_int24(const racs_int16 *src, racs_int24 *dst, size_t n) {
    size_t i = 0, m = n & ~7;
    racs_uint8 *_dst = (racs_uint8 *) dst;


    __m256i _pack_mask = _mm256_setr_epi8(
        0, 1, 2,  4, 5, 6,  8, 9, 10, 12, 13, 14, -1, -1, -1, -1, 
        0, 1, 2,  4, 5, 6,  8, 9, 10, 12, 13, 14, -1, -1, -1, -1  
    );

    __m256i _permute_mask = _mm256_setr_epi32(0, 1, 2, 4, 5, 6, 7, 7);

    for ( ; i < m; i += 8) {
        __m128i _in = _mm_loadu_si128((const __m128i *)(src + i));
        __m256i _i = _mm256_cvtepi16_epi32(_in);

        _i = _mm256_slli_epi32(_i, 8);

        __m256i _p = _mm256_shuffle_epi8(_i, _pack_mask);
        __m256i _out = _mm256_permutevar8x32_epi32(_p, _permute_mask);

        __m128i _l = _mm256_castsi256_si128(_out);
        __m128i _h = _mm256_extracti128_si256(_out, 1);

        _mm_storeu_si128((__m128i *)(_dst + i * 3), _l);
        _mm_storel_epi64((__m128i *)(_dst + i * 3 + 16), _h);
    }

    for ( ; i < n; i++) {
        racs_int32 s = src[i] << 8;

        _dst[i * 3]     = s & 0xff;
        _dst[i * 3 + 1] = (s >> 8) & 0xff;
        _dst[i * 3 + 2] = (s >> 16) & 0xff;
    }
}

#endif 
