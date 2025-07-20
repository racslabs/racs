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

#ifndef RACS_SIMD_CUMSUM_AVX_F32_HPP
#define RACS_SIMD_CUMSUM_AVX_F32_HPP

#include "../../cumsum.hpp"

#ifdef __AVX2__

namespace racs::simd {

RACS_SIMD __m256 RACS_VECTORCALL _mm256_scan_ps(__m256 _a) {
    __m256 _b, _c;

    _b = _mm256_permute_ps(_a, _MM_SHUFFLE(2, 1, 0, 3));
    _c = _mm256_permute2f128_ps(_b, _b, 41);
    _a = _mm256_add_ps(_a, _mm256_blend_ps(_b, _c, 0x11));

    _b = _mm256_permute_ps(_a, _MM_SHUFFLE(1, 0, 3, 2));
    _c = _mm256_permute2f128_ps(_b, _b, 41);
    _a = _mm256_add_ps(_a, _mm256_blend_ps(_b, _c, 0x33));

    _a = _mm256_add_ps(_a, _mm256_permute2f128_ps(_a, _a, 41));

    return _a;
}

}

#endif

#endif //RACS_SIMD_CUMSUM_AVX_F32_HPP
