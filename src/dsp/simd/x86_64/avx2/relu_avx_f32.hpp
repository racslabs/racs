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

#ifndef RACS_SIMD_RELU_AVX_F32_HPP
#define RACS_SIMD_RELU_AVX_F32_HPP

#include "../../relu.hpp"

namespace racs::simd {

#ifdef __AVX2__

RACS_SIMD __m256 RACS_VECTORCALL _mm256_relu_ps(__m256 _x) {
    const __m256 _c0 = _mm256_set1_ps(0);

    __m256 _mask = _mm256_cmp_ps(_x, _c0, _CMP_LT_OS);
    return _mm256_blendv_ps(_x, _c0, _mask);
}

RACS_SIMD __m256 RACS_VECTORCALL _mm256_lrelu_ps(__m256 _x, float a) {
    const __m256 _c0 = _mm256_set1_ps(0);
    const __m256 _a = _mm256_set1_ps(a);

    __m256 _mask = _mm256_cmp_ps(_x, _c0, _CMP_LT_OS);
    __m256 _nx = _mm256_mul_ps(_x, _a);

    return _mm256_blendv_ps(_x, _nx, _mask);
}

#endif

}

#endif //RACS_SIMD_RELU_AVX_F32_HPP
