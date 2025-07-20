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

#ifndef RACS_SIMD_GENERIC_SSE_F32_HPP
#define RACS_SIMD_GENERIC_SSE_F32_HPP

#include "../../generic.hpp"

namespace racs::simd {

#ifdef __SSE4_1__

RACS_SIMD __m128 RACS_VECTORCALL _mm_cmul_ps(__m128 _a, __m128 _b) {
    const __m128 neg = _mm_setr_ps(1, -1, 1, -1);

    __m128 _c = _mm_mul_ps(_a, _b);
    _b = _mm_shuffle_ps(_b, _b, 0xb1);
    _b = _mm_mul_ps(_b, neg);

    __m128 _d = _mm_mul_ps(_a, _b);
    _c = _mm_hsub_ps(_c, _d);

    return _mm_shuffle_ps(_c, _c, 0xd8);
}
#endif

}

#endif //RACS_SIMD_GENERIC_SSE_F32_HPP
