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

#include "min_max_avx_f32.hpp"

namespace racs::simd {

#ifdef __AVX2__
float min(float *a, size_t n) {

    const float max = std::numeric_limits<float>::max();

    size_t i = 0;
    float min = max;

    if (n >= 8) {
        __m256 _min = _mm256_set1_ps(max);

        for ( ; i < n; i += 8) {
            __m256 _a = _mm256_loadu_ps(a + i);
            _min = _mm256_min_ps(_a, _min);
        }

        min = _mm256_rmin_ps(_min);
    }

    for ( ; i < n; ++i) {
        if (a[i] < min) {
            min = a[i];
        }
    }

    return min;
}

float max(float *a, size_t n) {

    const float min = std::numeric_limits<float>::min();

    size_t i = 0;
    float max = min;

    if (n >= 8) {
        __m256 _max = _mm256_set1_ps(min);

        for ( ; i < n; i += 8) {
            __m256 _a = _mm256_loadu_ps(a + i);
            _max = _mm256_max_ps(_a, _max);
        }

        max = _mm256_rmax_ps(_max);
    }

    for ( ; i < n; ++i) {
        if (a[i] > max) {
            max = a[i];
        }
    }

    return max;
}
#endif

}