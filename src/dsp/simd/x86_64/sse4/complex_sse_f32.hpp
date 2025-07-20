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

#ifndef RACS_SIMD_COMPLEX_SSE_F32_HPP
#define RACS_SIMD_COMPLEX_SSE_F32_HPP

#include "../../complex.hpp"

namespace racs::simd {

#ifdef __SSE4_1__

RACS_SIMD __m128 RACS_VECTORCALL _mm_cexp_ps(__m128 _x) {
    __m128 _re = _mm_cos_ps(_x);
    __m128 _im = _mm_sin_ps(_x);

    return _mm_blend_ps(_re, _im, 0b1010);
}

#endif

}


#endif //RACS_SIMD_COMPLEX_SSE_F32_HPP
