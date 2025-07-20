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

#ifndef RACS_SIMD_FFT_AVX_F32_HPP
#define RACS_SIMD_FFT_AVX_F32_HPP

#include "../../constants.hpp"
#include "../../trigonometric.hpp"
#include "../../fft.hpp"

namespace racs::simd {

#ifdef __AVX2__

RACS_SIMD __m256i RACS_VECTORCALL _mm256_bitreverse_epi32x(__m256i _a, int m) {
    const __m256i _mask = _mm256_set1_epi32(0x1);
    __m256i _b = _mm256_setzero_si256();

    for (size_t i = 0; i < m; i++) {
        _b = _mm256_slli_epi32(_b, 0x1);
        _b = _mm256_or_si256(_b, _mm256_and_si256(_a, _mask));
        _a = _mm256_srli_epi32(_a, 0x1);
    }

    return _b;
}
#endif

}

#endif //RACS_SIMD_FFT_AVX_F32_HPP
