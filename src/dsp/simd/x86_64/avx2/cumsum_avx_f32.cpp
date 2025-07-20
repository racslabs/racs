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

#include "cumsum_avx_f32.hpp"

namespace racs::simd {

#ifdef __AVX2__

void cumsum(const float *a, float *b, size_t n) {
    size_t i = 0;

    if (n >= 8) {

        __m256 _c = _mm256_setzero_ps();

        for ( ; i < n; i += 8) {
            __m256 _a, _b;

            _a = _mm256_load_ps(a + i);
            _b = _mm256_scan_ps(_a);
            _b = _mm256_add_ps(_b, _c);

            _mm256_store_ps(b + i, _b);

            _b = _mm256_permute2f128_ps(_b, _b, 0x11);
            _c = _mm256_permute_ps(_b, 0xff);
        }
    }

    if (n < 8) {
        i = 1;
        b[0] = a[0];
    }

    for ( ; i < n; ++i) {
        b[i] = a[i] + b[i - 1];
    }
}

#endif

}