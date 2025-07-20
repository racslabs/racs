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

#include "dct_avx_f32.hpp"

namespace racs::simd {

#ifdef __AVX2__

void dct_even_odd(float* x, float* y, size_t n) {
    for (int i = 0; i < n; i += 8) {
        __m256 _x = _mm256_load_ps(x + i);
        _x = _mm256_evenodd_ps(_x);

        __m128 _even = _mm256_extractf128_ps(_x, 0);
        __m128 _odd = _mm256_extractf128_ps(_x, 1);

        auto m = i >> 0x1;

        _mm_store_ps(y + m, _even);
        _mm_store_ps(y + n - m - 4, _odd);
    }
}

void idct_even_odd(float* x, float* y, size_t n) {
    for (int i = 0; i < n / 2; i += 4) {
        __m128 _even = _mm_loadu_ps(x + i);
        __m128 _odd = _mm_loadu_ps(x + n - i - 4);

        __m256 _x = _mm256_evenodd_ps(_even, _odd);

        auto m = i << 0x1;

        _mm256_storeu_ps(y + m, _x);
    }
}
#endif

}