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

#ifndef RACS_SIMD_DCT_AVX_F32_HPP
#define RACS_SIMD_DCT_AVX_F32_HPP

#include "../../dct.hpp"

namespace racs::simd {

#ifdef __AVX2__

RACS_SIMD __m256 RACS_VECTORCALL _mm256_evenodd_ps(__m256 _x) {
    const __m256i _ctrl = _mm256_setr_epi32(0, 2, 4, 6, 7, 5, 3, 1);
    return _mm256_permutevar8x32_ps(_x, _ctrl);
}

RACS_SIMD __m256 RACS_VECTORCALL _mm256_evenodd_ps(__m128 _x, __m128 _y) {
    const __m256i _ctrl = _mm256_setr_epi32(0, 7, 1, 6, 2, 5, 3, 4);

    __m256 _z = _mm256_insertf128_ps(_mm256_castps128_ps256(_x), _y, 1);
    return _mm256_permutevar8x32_ps(_z, _ctrl);
}

#endif

}
#endif //RACS_SIMD_DCT_AVX_F32_HPP
