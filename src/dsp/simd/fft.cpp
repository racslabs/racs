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

#include "fft.hpp"

namespace racs::simd {

void butterfly1x32(std::complex<float>* x,  size_t n) {
    for (size_t i = 0; i < n; i += 2) {
        auto t = x[i + 1];
        auto u = x[i];

        x[i] = u + t;
        x[i + 1] = u - t;
    }
}

void bit_reverse_permute(std::complex<float>* x, std::complex<float>* y, int n, int m) {
    #ifdef __AVX2__
    if (n >= 8) {
        bit_reverse_permute8x32(x, y, n, m);
        return;
    }
    #endif

    #ifdef __SSE4_1__
    if (n >= 4) {
        bit_reverse_permute4x32(x, y, n, m);
        return;
    }
    #endif

    #ifdef __ARM_NEON__
    if (n >= 4) {
        bit_reverse_permute4x32(x, y, n, m);
        return;
    }
    #endif
}

void twiddle(std::complex<float>* x, FFT_TYPE type, size_t n) {
    #ifdef __AVX2__
    if (n >= 4) {
        twiddle4x32(x, type, n);
        return;
    }
    #endif

    #ifdef __SSE4_1__
    if (n >= 2) {
        twiddle2x32(x, type, n);
        return;
    }
    #endif

    #ifdef __ARM_NEON__
    if (n >= 2) {
        twiddle2x32(x, type, n);
        return;
    }
    #endif
}

void butterfly(std::complex<float>* x, std::complex<float> w, size_t n, size_t m) {
    auto k = m >> 0x1;

    #ifdef __AVX2__
    if (k >= 4) {
        butterfly4x32(x, w, n, m);
        return;
    }
    #endif

    #ifdef __SSE4_1__
    if (k >= 2) {
        butterfly2x32(x, w, n, m);
        return;
    }
    #endif

    #ifdef __ARM_NEON__
    if (k >= 2) {
        butterfly2x32(x, w, n, m);
        return;
    }
    #endif

    butterfly1x32(x, n);
}

void radix2(std::complex<float>* x, std::complex<float>* y, std::complex<float>* w, int n) {
    auto p = static_cast<int>(std::log2(n));

    bit_reverse_permute(x, y, n, p);

    for (int i = 1; i <= p; ++i) {
        auto wm = w[i - 1];
        auto m = 0x1 << i;
        butterfly(y, wm, n, m);
    }
}

}
