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

#include "../../convolution.hpp"
#include <iostream>

namespace racs::simd {

#ifdef __AVX2__

void convolve(float* a, float* b, float* c, size_t n, size_t m) {
    size_t i = 0;

    if (n >= 8) {
        for ( ; i < n; i += 8) {
            for (size_t j = 0; j < m; ++j) {
                __m256 _a = _mm256_loadu_ps(a + i + j);
                __m256 _b = _mm256_broadcast_ss(b + j);
                __m256 _c = _mm256_loadu_ps(c + i);

                _c = _mm256_fmadd_ps(_a, _b, _c);
                _mm256_storeu_ps(c + i, _c);
            }
        }
    }

    for ( ; i < n; ++i) {
        for (size_t j = 0; j < m; ++j) {
            c[i] += a[i + j] * b[j];
        }
    }
}

void transposed_convolver(float* a, float* b, float* c, size_t n, size_t m, size_t s) {
    for (size_t i = 0; i < n; ++i) {
        size_t j = 0;
        size_t k = i * s;
        size_t h = m % 8;

        __m256 _a = _mm256_broadcast_ss(a + i);

        if (m >= 8) {
            for ( ; j < m - h; j += 8) {
                __m256 _b = _mm256_loadu_ps(b + j);
                __m256 _c = _mm256_loadu_ps(c + k + j);
                _c = _mm256_fmadd_ps(_a, _b, _c);

                _mm256_storeu_ps(c + k + j, _c);
            }
        }

        for ( ; j < m; ++j) {
            c[k + j] += a[i] * b[j];
        }
    }
}
#endif

}
