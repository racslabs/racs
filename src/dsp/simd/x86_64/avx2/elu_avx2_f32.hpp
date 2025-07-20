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

#ifndef RACS_SIMD_ELU_AVX2_F32_HPP
#define RACS_SIMD_ELU_AVX2_F32_HPP

#include "../../elu.hpp"

namespace racs::simd {

#ifdef __AVX2__

RACS_SIMD __m256 RACS_VECTORCALL _mm256_elu_ps(__m256 _x, float a) {
    const __m256 _c0 = _mm256_set1_ps(0);
    const __m256 _c1 = _mm256_set1_ps(1);
    const __m256 _a = _mm256_set1_ps(a);

    __m256 _mask = _mm256_cmp_ps(_x, _c0, _CMP_LT_OS);
    __m256 _xn = _mm256_exp_ps(_x);

    _xn = _mm256_sub_ps(_xn, _c1);
    _xn = _mm256_mul_ps(_a, _xn);

    return _mm256_blendv_ps(_x, _xn, _mask);
}
#endif

}


#endif //RACS_SIMD_ELU_AVX2_F32_HPP
