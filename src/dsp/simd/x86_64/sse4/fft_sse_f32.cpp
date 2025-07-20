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

#include "fft_sse_f32.hpp"
#include "complex_sse_f32.hpp"

namespace racs::simd {

#ifdef __SSE4_1__

void bit_reverse_permute4x32(std::complex<float>* x, std::complex<float>* y, int n, int m) {
    const __m128i _b = _mm_setr_epi32(0, 1, 2, 3);
    union { __m128i _l; int l[4]; } u{};

    for (int i = 0; i < n; i += 4) {
        __m128i _a = _mm_set1_epi32(i);
        __m128i _c = _mm_add_epi32(_a, _b);

        u._l = _mm_bitreverse_epi32x(_c, m);
        y[i + 0] = x[u.l[0]];
        y[i + 1] = x[u.l[1]];
        y[i + 2] = x[u.l[2]];
        y[i + 3] = x[u.l[3]];
    }
}

void twiddle2x32(std::complex<float>* x, FFT_TYPE type, size_t n) {
    const __m128 _c0 = _mm_set1_ps(0x4);
    const __m128 _c1 = _mm_set1_ps(6.28318530718);

    __m128 _m = _mm_setr_ps(-0x2, -0x2, -0x4, -0x4);

    if (type == FFT_TYPE::INVERSE) {
        _m = _mm_setr_ps(0x2, 0x2, 0x4, 0x4);
    }

    for (int i = 0; i < n; i += 2) {
        __m128 _x = _mm_div_ps(_c1, _m);
        __m128 _y = _mm_cexp_ps(_x);

        _mm_storeu_ps(reinterpret_cast<float *>(x + i), _y);

        _m = _mm_mul_ps(_m, _c0);
    }
}

void butterfly2x32(std::complex<float>* x, std::complex<float> w, size_t n, size_t m) {
    alignas(32) std::complex<float> ww[2][2] = {
            {1, w},
            {w, w}
    };

    __m128 _w0 = _mm_loadu_ps(reinterpret_cast<float *>(ww[0]));
    __m128 _w1 = _mm_loadu_ps(reinterpret_cast<float *>(ww[1]));
    __m128 _wm = _mm_cmul_ps(_w1, _w1);

    for (size_t i = 0; i < n; i += m) {
        __m128 _ww = _w0;
        auto k = m >> 0x1;

        for (size_t j = 0; j < k; j += 2) {
            __m128 _x = _mm_loadu_ps(reinterpret_cast<float *>(x + i + j + k));
            __m128 _u = _mm_loadu_ps(reinterpret_cast<float *>(x + i + j));
            __m128 _t = _mm_cmul_ps(_x, _ww);

            _mm_storeu_ps(reinterpret_cast<float *>(x + i + j), _mm_add_ps(_u, _t));
            _mm_storeu_ps(reinterpret_cast<float *>(x + i + j + k), _mm_sub_ps(_u, _t));
            _ww = _mm_cmul_ps(_ww, _wm);
        }
    }
}
#endif

}