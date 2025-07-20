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

#include "linalg_avx_f32.hpp"

namespace racs::simd {

#ifdef __AVX2__

float dot(float* a, float* b, size_t n) {
    size_t i = 0;
    float sum = 0;

    if (n >= 8) {
        for (; i < n; i += 8) {
            __m256 _a = _mm256_loadu_ps(a + i);
            __m256 _b = _mm256_loadu_ps(b + i);

            sum += _mm256_dot_ps(_a, _b);
        }
    }

    for ( ; i < n; ++i) {
        sum += (a[i] * b[i]);
    }

    return sum;
}

float norm(float* x, size_t n) {
    size_t i = 0;
    float sum = 0;

    if (n >= 8) {
        for (; i < n; i += 8) {
            __m256 _x = _mm256_loadu_ps(x + i);
            sum += _mm256_norm_ps(_x);
        }
    }

    for ( ; i < n; ++i) {
        sum += (x[i] * x[i]);
    }

    return sqrtf(sum);
}
#endif

}
