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

#include "fft_avx_f32.hpp"
#include "complex_avx_f32.hpp"

namespace racs::simd {

#ifdef __AVX2__

void bit_reverse_permute8x32(std::complex<float>* x, std::complex<float>* y, int n, int m) {
    const __m256i _b = _mm256_setr_epi32(0, 1, 2, 3, 4, 5, 6, 7);
    union { __m256i _l; int l[8]; } u{};

    for (int i = 0; i < n; i += 8) {
        __m256i _a = _mm256_set1_epi32(i);
        __m256i _c = _mm256_add_epi32(_a, _b);
        u._l = _mm256_bitreverse_epi32x(_c, m);
        y[i + 0] = x[u.l[0]];
        y[i + 1] = x[u.l[1]];
        y[i + 2] = x[u.l[2]];
        y[i + 3] = x[u.l[3]];
        y[i + 4] = x[u.l[4]];
        y[i + 5] = x[u.l[5]];
        y[i + 6] = x[u.l[6]];
        y[i + 7] = x[u.l[7]];
    }
}

void twiddle4x32(std::complex<float>* x, FFT_TYPE type, size_t n) {
    const __m256 _c0 = _mm256_set1_ps(0x10);
    const __m256 _c1 = _mm256_set1_ps(6.28318530718);

    __m256 _m = _mm256_setr_ps(-0x2, -0x2, -0x4, -0x4, -0x8, -0x8, -0x10, -0x10);

    if (type == FFT_TYPE::INVERSE) {
        _m = _mm256_setr_ps(0x2, 0x2, 0x4, 0x4, 0x8, 0x8, 0x10, 0x10);
    }

    for (int i = 0; i < n; i += 4) {
        __m256 _x = _mm256_div_ps(_c1, _m);
        __m256 _y = _mm256_cexp_ps(_x);

        _mm256_storeu_ps(reinterpret_cast<float *>(x + i), _y);

        _m = _mm256_mul_ps(_m, _c0);
    }
}

void butterfly4x32(std::complex<float>* x, std::complex<float> w, size_t n, size_t m) {
    alignas(32) std::complex<float> ww[4][4] = {
            { 1, 1, 1, w },
            { 1, 1, w, w },
            { 1, w, w, w },
            { w, w, w, w }
    };

    const __m256 _w0 = _mm256_loadu_ps(reinterpret_cast<float *>(ww[0]));
    const __m256 _w1 = _mm256_loadu_ps(reinterpret_cast<float *>(ww[1]));
    const __m256 _w2 = _mm256_loadu_ps(reinterpret_cast<float *>(ww[2]));
    const __m256 _w3 = _mm256_loadu_ps(reinterpret_cast<float *>(ww[3]));

    __m256 _w = _mm256_cmul_ps(_w2, _w1);
    _w = _mm256_cmul_ps(_w, _w0);

    __m256 _wm = _mm256_cmul_ps(_w3, _w3);
    _wm = _mm256_cmul_ps(_wm, _w3);
    _wm = _mm256_cmul_ps(_wm, _w3);

    for (size_t i = 0; i < n; i += m) {
        __m256 _ww = _w;
        auto k = m >> 0x1;

        for (int j = 0; j < k; j += 4) {
            __m256 _x = _mm256_loadu_ps(reinterpret_cast<float *>(x + i + j + k));
            __m256 _u = _mm256_loadu_ps(reinterpret_cast<float *>(x + i + j));
            __m256 _t = _mm256_cmul_ps(_x, _ww);

            _mm256_storeu_ps(reinterpret_cast<float *>(x + i + j), _mm256_add_ps(_u, _t));
            _mm256_storeu_ps(reinterpret_cast<float *>(x + i + j + k), _mm256_sub_ps(_u, _t));
            _ww = _mm256_cmul_ps(_ww, _wm);
        }
    }
}
#endif

}