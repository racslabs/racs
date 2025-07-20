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

#include "biquad_avx_f32.hpp"

namespace racs::simd {

#if defined(__AVX2__) || defined(__SSE4_1__)

void biquadInit_config(const float* c, Biquad_config_f32* config) {
    float a1    = c[4];
    float a12   = a1 * a1;
    float a13   = a12 * a1;
    float a14   = a13 * a1;
    float a2    = c[5];
    float a22   = a2 * a2;

    __m128* c_ = config->c_;

    c_[0] = _mm_set_ps(      a14,       -a13,       a12,        -a1);
    c_[1] = _mm_set_ps(   a13*a2,    -a12*a2,     a1*a2,        -a2);
    c_[2] = _mm_set_ps(     -a13,        a12,       -a1,          0);
    c_[3] = _mm_set_ps(-3*a12*a2,    2*a1*a2,       -a2,          0);
    c_[4] = _mm_set_ps(      a12,        -a1,        0,           0);
    c_[5] = _mm_set_ps(  2*a1*a2,        -a2,        0,           0);
    c_[6] = _mm_set_ps(-2*a1*a22,        a22,        0,           0);
    c_[7] = _mm_set_ps(      -a1,          0,        0,           0);
    c_[8] = _mm_set_ps(      -a2,          0,        0,           0);
    c_[9] = _mm_set_ps(      a22,          0,        0,           0);
}

#endif

#ifdef __AVX2__

void biquadFIR(float* x, float* y, const float *c, size_t n) {
    size_t i = 0;

    __m256 _x0 = _mm256_loadu_ps(x);
    __m256 _x1 = _mm256_setr_ps(0, x[0], x[1], x[2], x[3], x[4], x[5], x[6]);
    __m256 _x2 = _mm256_setr_ps(0, 0   , x[0], x[1], x[2], x[3], x[4], x[5]);

    const __m256 _b0 = _mm256_broadcast_ss(c + 0);
    const __m256 _b1 = _mm256_broadcast_ss(c + 1);
    const __m256 _b2 = _mm256_broadcast_ss(c + 2);

    if (n >= 8) {
        i = 8;
        _mm256_storeu_ps(y, _mm256_biquadfir_ps(_x0, _x1, _x2, _b0, _b1, _b2));
    }

    if (n >= 16) {
        for ( ; i < n; i += 8) {
            _x0 = _mm256_loadu_ps(x + i);
            _x1 = _mm256_loadu_ps(x + i - 1);
            _x2 = _mm256_loadu_ps(x + i - 2);

            _mm256_storeu_ps(y + i, _mm256_biquadfir_ps(_x0, _x1, _x2, _b0, _b1, _b2));
        }
    }

    for ( ; i < n; ++i) {
        y[i] = c[0] * x[i] + c[1] * x[i - i] + c[2] * x[i - 2];
    }
}

void biquadIIR(float* y, const float* c, Biquad_config_f32* config, size_t n) {
    size_t i = 0;

    __m128* _c = config->c_;
    __m128 _wk1 = _mm_set1_ps(0);
    __m128 _wk2 = _mm_set1_ps(0);

    __m128 _x = _mm_loadu_ps(y + 0);
    __m128 _xk = _mm_broadcast_ss(y + 0);
    __m128 _xk1 = _mm_broadcast_ss(y + 1);
    __m128 _xk2 = _mm_broadcast_ss(y + 2);

    if (n >= 4) {
        i = 4;
        _mm_storeu_ps(y + 0, _mm_biquadiir_ps(_x, _xk, _xk1, _xk2, _wk1, _wk2, _c));
    }

    if (n >= 8) {
        for ( ; i < n; i += 4) {
            _wk1 = _mm_broadcast_ss(y + i - 1);
            _wk2 = _mm_broadcast_ss(y + i - 2);

            _x = _mm_loadu_ps(y + i);
            _xk = _mm_broadcast_ss(y + i);
            _xk1 = _mm_broadcast_ss(y + i + 1);
            _xk2 = _mm_broadcast_ss(y + i + 2);

            _mm_storeu_ps(y + i, _mm_biquadiir_ps(_x, _xk, _xk1, _xk2, _wk1, _wk2, _c));
        }
    }

    for ( ; i < n; ++i) {
        y[i] = y[i] + c[4] * y[i - 1] + c[5] * y[i - 2];
    }

}

#endif

}