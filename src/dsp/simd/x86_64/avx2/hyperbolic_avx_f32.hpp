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

#ifndef RACS_SIMD_HYPERBOLIC_AVX_F32_HPP
#define RACS_SIMD_HYPERBOLIC_AVX_F32_HPP

#include "../../hyperbolic.hpp"

namespace racs::simd {

#ifdef __AVX2__

RACS_SIMD __m256 RACS_VECTORCALL _mm256_sinh_ps(__m256 _x) {
    const __m256 _c0 = _mm256_set1_ps(0.5);
    const __m256 _neg = _mm256_set1_ps(-1);

    __m256 _nx = _mm256_mul_ps(_x, _neg);
    __m256 _a = _mm256_exp_ps(_x);
    __m256 _b = _mm256_exp_ps(_nx);

    __m256 _y = _mm256_sub_ps(_a, _b);

    return _mm256_mul_ps(_y, _c0);
}

RACS_SIMD __m256 RACS_VECTORCALL _mm256_cosh_ps(__m256 _x) {
    const __m256 _c0 = _mm256_set1_ps(0.5);
    const __m256 _neg = _mm256_set1_ps(-1);

    __m256 _nx = _mm256_mul_ps(_x, _neg);
    __m256 _a = _mm256_exp_ps(_x);
    __m256 _b = _mm256_exp_ps(_nx);
    __m256 _y = _mm256_add_ps(_a, _b);

    return _mm256_mul_ps(_y, _c0);
}

RACS_SIMD __m256 RACS_VECTORCALL _mm256_tanh_ps(__m256 _x) {
    __m256 _a = _mm256_sinh_ps(_x);
    __m256 _b = _mm256_cosh_ps(_x);
    return _mm256_div_ps(_a, _b);
}

#endif

}

#endif //RACS_SIMD_HYPERBOLIC_AVX_F32_HPP
