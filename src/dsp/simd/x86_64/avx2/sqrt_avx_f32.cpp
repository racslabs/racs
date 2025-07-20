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

#include "../../sqrt.hpp"

namespace racs::simd {

#ifdef __AVX2__

void sqrt(float* x, float* y, size_t n) {
    size_t i = 0;

    if (n >= 8) {
        for (; i < n; i += 8) {
            __m256 _x = _mm256_load_ps(x + i);
            __m256 _y = _mm256_sqrt_ps(_x);
            _mm256_store_ps(y + i, _y);
        }
    }

    for ( ; i < n; ++i) {
        y[i] = sqrtf(x[i]);
    }
}
#endif

}