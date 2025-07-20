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

#ifndef RACS_SIMD_LOGARITHMIC_AVX_F32_HPP
#define RACS_SIMD_LOGARITHMIC_AVX_F32_HPP

#include "../../constants.hpp"

namespace racs::simd {

#ifdef __AVX2__

RACS_SIMD __m256 RACS_VECTORCALL _mm256_log_ps(__m256 _x) {
    const __m256i _c0 = _mm256_set1_epi32(127);
    const __m256i _c1 = _mm256_set1_epi32(8388607);
    const __m256i _c2 = _mm256_set1_epi32(1065353216);
    const __m256 _c3 = _mm256_set1_ps(0.6931471806);

    const __m256 _s1 = _mm256_set1_ps(-1.74178);
    const __m256 _s2 = _mm256_set1_ps(2.82117);
    const __m256 _s3 = _mm256_set1_ps(-1.46994);
    const __m256 _s4 = _mm256_set1_ps(0.447178);
    const __m256 _s5 = _mm256_set1_ps(-0.0565717);

    union {__m256i_u _i; __m256 _f; } u{._f = _x };

    __m256i _bx = u._i;
    __m256i _ex = _mm256_srli_epi32(_bx, 0x17);
    __m256i _t = _mm256_sub_epi32(_ex, _c0);

    _bx = _mm256_and_si256(_bx, _c1);
    _bx = _mm256_or_si256(_bx, _c2);

    u._i = _bx;
    _x =  u._f;

    __m256 _d = _mm256_fmadd_ps(_x, _s5, _s4);
    _d = _mm256_fmadd_ps(_x, _d, _s3);
    _d = _mm256_fmadd_ps(_x, _d, _s2);
    _d = _mm256_fmadd_ps(_x, _d, _s1);

    __m256 _t0 = _mm256_cvtepi32_ps(_t);

    return _mm256_fmadd_ps(_t0, _c3, _d);
}

RACS_SIMD __m256 RACS_VECTORCALL _mm256_log2_ps(__m256 _x) {
    const __m256 _c0 = _mm256_set1_ps(1.4426950409);
    return _mm256_mul_ps(_mm256_log_ps(_x), _c0);
}

RACS_SIMD __m256 RACS_VECTORCALL _mm256_log10_ps(__m256 _x) {
    const __m256 _c0 = _mm256_set1_ps(0.4342944819);
    return _mm256_mul_ps(_mm256_log_ps(_x), _c0);
}
#endif

}
#endif //RACS_SIMD_LOGARITHMIC_AVX_F32_HPP
