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

#include "fft_neon_f32.hpp"

namespace racs::simd {

#ifdef __ARM_NEON__

void bit_reverse_permute4x32(std::complex<float>* x, std::complex<float>* y, int n, int m) {
    const v128i _b = { 0, 1, 2, 3 };

    union { v128i _l; int l[4]; } u{};

    for (int i = 0; i < n; i += 4) {
        v128i _a = vdupq_n_s32(i);
        v128i _c = vaddq_s32(_a, _b);

        u._l = vbitreverseq_s32(_c, m);
        y[i + 0] = x[u.l[0]];
        y[i + 1] = x[u.l[1]];
        y[i + 2] = x[u.l[2]];
        y[i + 3] = x[u.l[3]];
    }
}

void twiddle2x32(std::complex<float>* x, FFT_TYPE type, size_t n) {
    const v128 _c0 = vdupq_n_f32(0x4);
    const v128 _c1 = vdupq_n_f32(6.28318530718);

    v128 _m = { -0x2, -0x2, -0x4, -0x4 };

    if (type == FFT_TYPE::INVERSE) {
        v128 _neg = vdupq_n_f32(-1);
        _m = vmulq_f32(_m, _neg);
    }

    for (int i = 0; i < n; i += 2) {
        v128 _x = vdivq_f32(_c1, _m);
        v128 _y = vcexpq_f32(_x);

        vst1q_f32(reinterpret_cast<float *>(x + i), _y);
        _m = vmulq_f32(_m, _c0);
    }
}

void butterfly2x32(std::complex<float>* x, std::complex<float> w, size_t n, size_t m) {
    std::complex<float> ww[2][2] = {
            {1, w },
            {w, w }
    };

    v128 _w0 = vld1q_f32(reinterpret_cast<float *>(ww[0]));
    v128 _w1 = vld1q_f32(reinterpret_cast<float *>(ww[1]));
    v128 _wm = vcmulq_f32(_w1, _w1);

    for (size_t i = 0; i < n; i += m) {
        v128 _ww = _w0;
        auto k = m >> 0x1;

        for (size_t j = 0; j < k; j += 2) {
            v128 _x = vld1q_f32(reinterpret_cast<float *>(x + i + j + k));
            v128 _u = vld1q_f32(reinterpret_cast<float *>(x + i + j));
            v128 _t = vcmulq_f32(_x, _ww);

            vst1q_f32(reinterpret_cast<float *>(x + i + j), vaddq_f32(_u, _t));
            vst1q_f32(reinterpret_cast<float *>(x + i + j + k), vsubq_f32(_u, _t));
            _ww = vcmulq_f32(_ww, _wm);
        }
    }
}
#endif

}