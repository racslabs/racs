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

#ifndef RACS_SIMD_MIN_MAX_AVX_F32_HPP
#define RACS_SIMD_MIN_MAX_AVX_F32_HPP

#include "../../min_max.hpp"

namespace racs::simd {

#ifdef __AVX2__

RACS_SIMD float RACS_VECTORCALL _mm256_rmin_ps(__m256 _x) {
    __m128 _y = _mm256_extractf128_ps(_x, 1);
    _y = _mm_min_ps(_y, _mm256_castps256_ps128(_x));
    _y = _mm_min_ps(_y, _mm_movehl_ps(_y, _y));
    _y = _mm_min_ss(_y, _mm_movehdup_ps(_y));

    return _mm_cvtss_f32(_y);
}

RACS_SIMD float RACS_VECTORCALL _mm256_rmax_ps(__m256 _x) {
    __m128 _y = _mm256_extractf128_ps(_x, 1);
    _y = _mm_max_ps(_y, _mm256_castps256_ps128(_x));
    _y = _mm_max_ps(_y, _mm_movehl_ps(_y, _y));
    _y = _mm_max_ss(_y, _mm_movehdup_ps(_y));

    return _mm_cvtss_f32(_y);
}
#endif

}

#endif //RACS_SIMD_MIN_MAX_AVX_F32_HPP
