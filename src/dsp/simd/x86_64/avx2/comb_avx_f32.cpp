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

#include "../../comb.hpp"

namespace racs::simd {

#ifdef __AVX2__

void ffcomb(float* a, float* ap, float* b, float g, size_t n) {
    size_t i = 0;

    const __m256 _g = _mm256_set1_ps(g * 0.4f);

    if (n >= 8) {
        for ( ; i < n; i += 8) {
            __m256 _a = _mm256_loadu_ps(a + i);
            __m256 _ap = _mm256_loadu_ps(ap + i);
            __m256 _b = _mm256_fmadd_ps(_ap, _g, _a);

            _mm256_storeu_ps(b + i, _b);
        }
    }

    for ( ; i < n; ++i) {
        b[i] = a[i] + g * ap[i];
    }

}

void fbcomb(float* a, float* b, float g, size_t n, size_t m) {
    const __m256 _g = _mm256_set1_ps(g * 0.4f);

    size_t i = 0;

    if (m >= 8) {
        for ( ; i < m; i += 8) {
            __m256 _a = _mm256_loadu_ps(a + i);
            _mm256_storeu_ps(b + i, _a);
        }
    }

    for ( ; i < m; ++i) {
        b[i] = a[i];
    }

    if (n - m >= 8 && m >= 8) {
        for ( ; i < n; i += 8) {
            __m256 _a = _mm256_loadu_ps(a + i);
            __m256 _b = _mm256_loadu_ps(b + i - m);
            _b = _mm256_fmadd_ps(_g, _b, _a);
            _mm256_storeu_ps(b + i, _b);
        }
    }

    for ( ; i < n; ++i) {
        b[i] = a[i] + g * b[i - m];
    }
}

void apcomb(float* a, float* ap, float* b, float g, size_t n, size_t m) {
    size_t i = 0;

    const __m256 _g = _mm256_set1_ps(g * 0.4f);

    // first pass
    if (n >= 8) {
        for ( ; i < n; i += 8) {
            __m256 _a = _mm256_loadu_ps(a + i);
            __m256 _ap = _mm256_loadu_ps(ap + i);
            __m256 _b = _mm256_fmadd_ps(_a, _g, _ap);

            _mm256_storeu_ps(b + i, _b);
        }
    }

    for ( ; i < n; ++i) {
        b[i] = g * a[i] + ap[i];
    }

    //second pass
    i = m;

    if (n - m >= 8 && m >= 8) {
        for ( ; i < n; i += 8) {
            __m256 _a = _mm256_loadu_ps(b + i);
            __m256 _b = _mm256_loadu_ps(b + i - m);
            _b = _mm256_fnmadd_ps(_g, _b, _a);
            _mm256_storeu_ps(b + i, _b);
        }
    }

    for ( ; i < n; ++i) {
        b[i] = b[i] - g * b[i - m];
    }
}

#endif

}
