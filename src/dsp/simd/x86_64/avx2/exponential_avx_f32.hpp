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

#ifndef RACS_SIMD_EXPONENTIAL_AVX_F32_HPP
#define RACS_SIMD_EXPONENTIAL_AVX_F32_HPP

#include "../../logarithmic.hpp"
#include "../../trigonometric.hpp"
#include <cmath>

namespace racs::simd {

#ifdef __AVX2__

RACS_SIMD __m256 RACS_VECTORCALL _mm256_exp_ps(__m256 _x) {
    const __m256 _c0 = _mm256_set1_ps(1.44269504089);
    const __m256 _c1 = _mm256_set1_ps(8388608);
    const __m256 _c2 = _mm256_set1_ps(1065353216);

    const __m256 _s1 = _mm256_set1_ps(3.06852819440055e-1);
    const __m256 _s2 = _mm256_set1_ps(-2.40226506959101e-1);
    const __m256 _s3 = _mm256_set1_ps(-5.57129652016652e-2);
    const __m256 _s4 = _mm256_set1_ps(-9.01146535969578e-3);
    const __m256 _s5 = _mm256_set1_ps(-1.90188191959304e-3);

    union { __m256 _f; __m256i _i; } u{};

    __m256 _y = _mm256_mul_ps(_x, _c0);
    __m256 _yf = _mm256_sub_ps(_y, _mm256_floor_ps(_y));

    __m256 _d = _mm256_fmadd_ps(_yf, _s5, _s4);
    _d = _mm256_fmadd_ps(_yf, _d, _s3);
    _d = _mm256_fmadd_ps(_yf, _d, _s2);
    _d = _mm256_fmadd_ps(_yf, _d, _s1);
    _d = _mm256_mul_ps(_yf, _d);

    __m256 _yd = _mm256_sub_ps(_y, _d);
    __m256 _z = _mm256_fmadd_ps(_yd, _c1, _c2);

    u._i = _mm256_cvtps_epi32(_z);

    return u._f;
}

RACS_SIMD __m256 RACS_VECTORCALL _mm256_pow_ps(__m256 _b, __m256 _x) {
    __m256 _a = _mm256_mul_ps(_x, _mm256_log_ps(_b));
    return _mm256_exp_ps(_a);
}
#endif

}

#endif //RACS_SIMD_EXPONENTIAL_AVX_F32_HPP
