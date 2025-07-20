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

#ifndef RACS_SIMD_LINALG_AVX_F32_HPP
#define RACS_SIMD_LINALG_AVX_F32_HPP

#include "../../linalg.hpp"
#include <cmath>

namespace racs::simd {

#ifdef __AVX2__

RACS_SIMD float RACS_VECTORCALL _mm256_dot_ps(__m256 _a, __m256 _b) {
    __m256 _x = _mm256_dp_ps(_a, _b, 0xff);
    return _x[0] + _x[4];
}

RACS_SIMD float RACS_VECTORCALL _mm256_norm_ps(__m256 _x) {
    return _mm256_dot_ps(_x, _x);
}
#endif

}

#endif //RACS_SIMD_LINALG_AVX_F32_HPP
