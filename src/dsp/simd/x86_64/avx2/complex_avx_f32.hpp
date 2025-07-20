/*
 * Copyright (c) 2024 Computer Music Research Lab (https://cmrlab.ai)
 *
 * This file is part of ZForm.
 *
 * ZForm is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * ZForm is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with ZForm. If not, see <http://www.gnu.org/licenses/>.
 *
 * If you are using this library in a commercial, closed-source, or
 * proprietary project, you must purchase a ZForm license.
 */

#ifndef RACS_SIMD_COMPLEX_AVX_F32_HPP
#define RACS_SIMD_COMPLEX_AVX_F32_HPP

#include "../../complex.hpp"

namespace racs::simd {

#ifdef __AVX2__

RACS_SIMD __m256 RACS_VECTORCALL _mm256_csquare_ps(__m256 _x) {
    return _mm256_cmul_ps(_x, _x);
}

RACS_SIMD __m256 RACS_VECTORCALL _mm256_cconj_ps(__m256 _x) {
    const __m256 _c0 = _mm256_setr_ps(1, -1, 1, -1, 1, -1, 1, -1);
    return _mm256_mul_ps(_x, _c0);
}

RACS_SIMD __m256 RACS_VECTORCALL _mm256_real2cmplx_ps(__m128 _r) {
    const __m256i _ctrl = _mm256_setr_epi32(0, 4, 1, 5, 2, 6, 3, 7);

    __m256 _c = _mm256_setzero_ps();
    _c = _mm256_insertf128_ps(_c, _r, 0);

    return _mm256_permutevar8x32_ps(_c, _ctrl);
}

RACS_SIMD __m128 RACS_VECTORCALL _mm256_real_ps(__m256 _x) {
    const __m256i _ctrl = _mm256_setr_epi32(0, 2, 4, 6, 1, 3, 5, 7);
    __m256 _y = _mm256_permutevar8x32_ps(_x, _ctrl);
    return _mm256_extractf128_ps(_y, 0);
}

RACS_SIMD __m128 RACS_VECTORCALL _mm256_img_ps(__m256 _x) {
    const __m256i _ctrl = _mm256_setr_epi32(0, 2, 4, 6, 1, 3, 5, 7);
    __m256 _y = _mm256_permutevar8x32_ps(_x, _ctrl);
    return _mm256_extractf128_ps(_y, 1);
}

RACS_SIMD __m128 RACS_VECTORCALL _mm256_mag_ps(__m256 _x) {
    const __m256i _ctrl = _mm256_setr_epi32(0, 2, 4, 6, 1, 3, 5, 7);

    _x = _mm256_mul_ps(_x, _x);
    _x = _mm256_permutevar8x32_ps(_x, _ctrl);

    __m128 _re = _mm256_extractf128_ps(_x, 0);
    __m128 _im = _mm256_extractf128_ps(_x, 1);
    __m128 _y = _mm_add_ps(_re, _im);

    return _mm_sqrt_ps(_y);
}

RACS_SIMD __m256 RACS_VECTORCALL _mm256_cexp_ps(__m256 _x) {
    __m256 _re = _mm256_cos_ps(_x);
    __m256 _im = _mm256_sin_ps(_x);

    return _mm256_blend_ps(_re, _im, 0xaa);
}

RACS_SIMD __m256 RACS_VECTORCALL _mm256_cexpz_ps(__m256 _x) {
    __m256i _ctl0 = _mm256_setr_epi32(0, 0, 2, 2, 4, 4, 6, 6);
    __m256i _ctl1 = _mm256_setr_epi32(1, 1, 3, 3, 5, 5, 7, 7);

    __m256 _re = _mm256_permutevar8x32_ps(_x, _ctl0);
    __m256 _im = _mm256_permutevar8x32_ps(_x, _ctl1);

    _re = _mm256_exp_ps(_re);
    _im = _mm256_cexp_ps(_im);

    return _mm256_mul_ps(_re, _im);
}

RACS_SIMD __m256 RACS_VECTORCALL _mm256_csinh_ps(__m256 _x) {
    const __m256 _c0 = _mm256_set1_ps(-1);
    const __m256 _c1 = _mm256_set1_ps(2);

    __m256 _a = _mm256_cexpz_ps(_x);
    __m256 _b = _mm256_cexpz_ps(_mm256_mul_ps(_x, _c0));

    __m256 _c = _mm256_sub_ps(_a, _b);

    return _mm256_div_ps(_c, _c1);
}

RACS_SIMD __m256 RACS_VECTORCALL _mm256_csqrt_ps(__m256 _x) {
    const __m256 _c0 = _mm256_setzero_ps();
    const __m256 _c1 = _mm256_set1_ps(2);
    const __m256 _c2 = _mm256_set1_ps(1);

    const __m256i _ctl0 = _mm256_setr_epi32(0, 0, 2, 2, 4, 4, 6, 6);
    const __m256i _ctl1 = _mm256_setr_epi32(0, 4, 1, 5, 2, 6, 3, 7);

    __m256 _v = _mm256_cmp_ps(_x, _c0, _CMP_GT_OQ);
    _v = _mm256_andnot_ps(_v, _c2);
    _v = _mm256_fnmadd_ps(_v, _c1, _c2);

    __m256 _a = _mm256_permutevar8x32_ps(_x, _ctl0);
    __m256 _b = _mm256_blend_ps(_v, _c0, 0x55);
    __m128 _m = _mm256_mag_ps(_x);

    __m256 _z = _mm256_broadcast_ps(&_m);
    _z = _mm256_permutevar8x32_ps(_z, _ctl1);

    __m256 _re = _mm256_add_ps(_z, _a);
    __m256 _im = _mm256_sub_ps(_z, _a);

    _re = _mm256_div_ps(_re, _c1);
    _re = _mm256_sqrt_ps(_re);

    _im = _mm256_div_ps(_im, _c1);
    _im = _mm256_sqrt_ps(_im);
    _im = _mm256_mul_ps(_b, _im);

    return _mm256_blend_ps(_re, _im, 0xaa);
}

#endif

}

#endif //RACS_SIMD_COMPLEX_AVX_F32_HPP
