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

#ifndef RACS_SIMD_GENERIC_AVX_F32_HPP
#define RACS_SIMD_GENERIC_AVX_F32_HPP

#include "../../generic.hpp"

namespace racs::simd {

#ifdef __AVX2__

RACS_SIMD __m256 RACS_VECTORCALL _mm256_cmul_ps(__m256 _a, __m256 _b) {
    const __m256 _neg = _mm256_setr_ps(1, -1, 1, -1, 1, -1, 1, -1);

    __m256 _c = _mm256_mul_ps(_a, _b);
    _b = _mm256_shuffle_ps(_b, _b, 0xb1);
    _b = _mm256_mul_ps(_b,  _neg);

    __m256 _d = _mm256_mul_ps(_a, _b);
    _c = _mm256_hsub_ps(_c, _d);

    return _mm256_shuffle_ps(_c, _c, 0xd8);
}

RACS_SIMD __m256 RACS_VECTORCALL _mm256_cdiv_ps(__m256 _a, __m256 _b) {
    const __m256 _neg = _mm256_setr_ps(1, -1, 1, -1, 1, -1, 1, -1);
    const __m256i _ctl = _mm256_setr_epi32(0, 0, 2, 2, 4, 4, 6, 6);

    __m256 _bc = _mm256_mul_ps(_b, _neg);
    __m256 _n = _mm256_cmul_ps(_a, _bc);

    __m256 _d = _mm256_cmul_ps(_b, _bc);
    _d = _mm256_permutevar8x32_ps(_d, _ctl);

    return _mm256_div_ps(_n, _d);
}

RACS_SIMD __m256 RACS_VECTORCALL _mm256_mod_ps(__m256 _a, __m256 _b) {
    __m256 _c = _mm256_div_ps(_a, _b);
    _c = _mm256_floor_ps(_c);
    __m256 _d = _mm256_mul_ps(_c, _b);
    return _mm256_sub_ps(_a, _d);
}

RACS_SIMD __m256 RACS_VECTORCALL _mm256_rev_ps(__m256 _a) {
    const __m256i _ctrl = _mm256_setr_epi32(7, 6, 5, 4, 3, 2, 1, 0);
    return _mm256_permutevar8x32_ps(_a, _ctrl);
}

RACS_SIMD __m256 RACS_VECTORCALL _mm256_crev_ps(__m256 _a) {
    const __m256i _ctrl = _mm256_setr_epi32(6, 7, 4, 5, 2, 3, 0, 1);
    return _mm256_permutevar8x32_ps(_a, _ctrl);
}

RACS_SIMD __m256 RACS_VECTORCALL _mm256_dilatex2_ps(__m128 _x) {
    const __m256i _ctrl = _mm256_setr_epi32(0, 4, 1, 5, 2, 6, 3, 7);

    __m256 _y = _mm256_setzero_ps();
    _y = _mm256_insertf128_ps(_y, _x, 0);

    return _mm256_permutevar8x32_ps(_y, _ctrl);
}

#endif

}



#endif //RACS_SIMD_GENERIC_AVX_F32_HPP
