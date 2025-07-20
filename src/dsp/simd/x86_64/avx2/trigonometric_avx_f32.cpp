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

#include "../../trigonometric.hpp"

namespace racs::simd {

#ifdef __AVX2__

void cos(float *a, float *b, size_t n) {
    size_t i = 0;

    if (n >= 8) {
        for (; i < n; i += 8) {
            __m256 _a = _mm256_load_ps(a + i);
            _mm256_store_ps(b + i, _mm256_cos_ps(_a));
        }
    }

    for (; i < n; ++i) {
        b[i] = cosf(a[i]);
    }
}

void sin(float *a, float *b, size_t n) {
    size_t i = 0;

    if(n >= 8) {
        for (; i < n; i += 8) {
            __m256 _a = _mm256_load_ps(a + i);
            _mm256_store_ps(b + i, _mm256_sin_ps(_a));
        }
    }

    for (; i < n; ++i) {
        b[i] = sinf(a[i]);
    }
}

void sinc(float* a, float* b, size_t n) {
    size_t i = 0;

    if (n >= 8) {
        for (; i < n; i += 8) {
            __m256 _a = _mm256_load_ps(a + i);
            _mm256_store_ps(b + i, _mm256_sinc_ps(_a));
        }
    }

    for (; i < n; ++i) {
        auto x = a[i] * simd::pi<float>;

        if (x == 0) {
            b[i] = 1;
        } else {
            b[i] = sinf(x) / x;
        }
    }
}

void tan(float *a, float *b, size_t n) {
    size_t i = 0;

    if (n >= 8) {
        for (; i < n; i += 8) {
            __m256 _a = _mm256_load_ps(a + i);
            _mm256_store_ps(b + i, _mm256_tan_ps(_a));
        }
    }

    for (; i < n; ++i) {
        b[i] = tanf(a[i]);
    }
}

void cot(float *a, float *b, size_t n) {
    size_t i = 0;

    if (n >= 8) {
        for (; i < n; i += 8) {
            __m256 _a = _mm256_load_ps(a + i);
            _mm256_store_ps(b + i, _mm256_cot_ps(_a));
        }
    }

    for (; i < n; ++i) {
        b[i] = cosf(a[i]) / sinf(a[i]);
    }
}

void sec(float *a, float *b, size_t n) {
    size_t i = 0;

    if (n >= 8) {
        for (; i < n; i += 8) {
            __m256 _a = _mm256_load_ps(a + i);
            _mm256_store_ps(b + i, _mm256_sec_ps(_a));
        }
    }

    for (; i < n; ++i) {
        b[i] = 1.f / cosf(a[i]);
    }
}

void csc(float *a, float *b, size_t n) {
    size_t i = 0;

    if (n >= 8) {
        for (; i < n; i += 8) {
            __m256 _a = _mm256_load_ps(a + i);
            _mm256_store_ps(b + i, _mm256_csc_ps(_a));
        }
    }

    for (; i < n; ++i) {
        b[i] = 1.f / sinf(a[i]);
    }
}
#endif

}