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

#include "../../polynomial.hpp"

namespace racs::simd {

#ifdef __AVX2__

void polyval(float* p, float* x, float* y, size_t n, size_t m) {
    size_t i = 0;

    if (m >= 8) {
        for ( ; i < m; i += 8) {
            __m256 _x = _mm256_load_ps(x + i);
            __m256 _y = _mm256_broadcast_ss(p + 0);

            for (size_t j = 1; j < n; ++j) {
                __m256 _a = _mm256_broadcast_ss(p + j);
                _y = _mm256_fmadd_ps(_y, _x, _a);
            }

            _mm256_store_ps(y + i, _y);
        }
    }

    for ( ; i < m; ++i) {
        y[i] = p[0];

        RACS_UNROLL(4)
        for (size_t j = 1; j < n; ++j) {
            y[i] = y[i] * x[i] + p[j];
        }
    }
}

void polyval(std::complex<float>* p, std::complex<float>* x, std::complex<float>* y, size_t n, size_t m) {
    size_t i = 0;

    if (m >= 4) {
        for ( ; i < m; i += 4) {
            __m256 _x = _mm256_loadu_ps(reinterpret_cast<float*>(x + i));
            __m256 _u = _mm256_broadcast_ss(reinterpret_cast<float*>(p) + 0);
            __m256 _v = _mm256_broadcast_ss(reinterpret_cast<float*>(p) + 1);
            __m256 _y = _mm256_blend_ps(_u, _v, 0xaa);

            for (size_t j = 1; j < n; ++j) {
                _u = _mm256_broadcast_ss(reinterpret_cast<float*>(p + j) + 0);
                _v = _mm256_broadcast_ss(reinterpret_cast<float*>(p + j) + 1);

                __m256 _a = _mm256_blend_ps(_u, _v, 0xaa);

                _y = _mm256_cmul_ps(_y, _x);
                _y = _mm256_add_ps(_y, _a);
            }

            _mm256_storeu_ps(reinterpret_cast<float*>(y + i), _y);
        }
    }

    for ( ; i < m; ++i) {
        y[i] = p[0];

        RACS_UNROLL(2)
        for (size_t j = 1; j < n; ++j) {
            y[i] = y[i] * x[i] + p[j];
        }
    }
}

#endif

}
