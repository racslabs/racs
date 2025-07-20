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

#include "normal_avx_f32.hpp"

namespace racs::simd {

#ifdef __AVX2__

void normal(float* x, float* y, float mu, float sigma, size_t n) {
    const __m256 _mu = _mm256_set1_ps(mu);
    const __m256 _sigma = _mm256_set1_ps(sigma);
    const __m256 _c0 = _mm256_set1_ps(0.3989422804 * (1 / sigma));

    size_t i = 0;

    if (n >= 8) {
        for (; i < n; i += 8) {
            __m256 _x = _mm256_load_ps(x + i);
            __m256 _y = _mm256_normal_ps(_x, _mu, _sigma);
            _y = _mm256_mul_ps(_c0, _y);
            _mm256_store_ps(y + i, _y);
        }
    }

    for ( ; i < n; ++i) {
        y[i] = normal(x[i], mu, sigma);
    }
}

#endif

}
