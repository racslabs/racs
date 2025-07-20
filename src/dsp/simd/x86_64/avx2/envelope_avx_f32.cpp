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

#include "../../envelope.hpp"

namespace racs::simd {

#ifdef __AVX2__

void envelope(float* x, float level, float a0, size_t n) {
    alignas(32) float c[4][4] = {
            { 1, 2, 3, 4 },
            { 6, 3, 1, 0 },
            { 4, 1, 0, 0 },
            { 1, 0, 0, 0 }
    };

    const __m128 _a0 = _mm_set1_ps(a0);
    const __m128 _a1 = _mm_mul_ps(_a0, _a0);
    const __m128 _a2 = _mm_mul_ps(_a1, _a0);
    const __m128 _a3 = _mm_mul_ps(_a2, _a0);

    const __m128 _c0 = _mm_mul_ps(_a0, _mm_loadu_ps(c[0]));
    const __m128 _c1 = _mm_mul_ps(_a1, _mm_loadu_ps(c[1]));
    const __m128 _c2 = _mm_mul_ps(_a2, _mm_loadu_ps(c[2]));
    const __m128 _c3 = _mm_mul_ps(_a3, _mm_loadu_ps(c[3]));

    __m128 _x = _mm_set1_ps(level);
    __m128 _y = _mm_fmadd_ps(_x, _c0, _x);

    if (n >= 4) {
        _y = _mm_fmadd_ps(_x, _c1, _y);
        _y = _mm_fmadd_ps(_x, _c2, _y);
        _y = _mm_fmadd_ps(_x, _c3, _y);

        _mm_storeu_ps(x, _y);
    }

    size_t i = 4;

    if (n >= 8) {
        for ( ;i < n; i += 4) {
            _x = _mm_broadcast_ss(x + i - 1);
            _y = _mm_fmadd_ps(_x, _c0, _x);
            _y = _mm_fmadd_ps(_x, _c1, _y);
            _y = _mm_fmadd_ps(_x, _c2, _y);
            _y = _mm_fmadd_ps(_x, _c3, _y);

            _mm_storeu_ps(x + i, _y);
        }
    }

    for ( ; i < n ; ++i) {
        x[i] += x[i - 1] * level;
    }
}
#endif

}
