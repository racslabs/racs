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

#ifdef __ARM_NEON__

void polyval(float* p, float* x, float* y, size_t n, size_t m) {
    size_t i = 0;

    if (m >= 4) {
        for ( ; i < m; i += 4) {
            v128 _x = vld1q_f32(x + i);
            v128 _y = vdupq_n_f32(p[0]);

            for (size_t j = 1; j < n; ++j) {
                v128 _a = vdupq_n_f32(p[j]);
                _y = vmlaq_f32(_a, _y, _x);
            }

            vst1q_f32(y + i, _y);
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

    v128u _mask = { 0xffffffff, 0x00000000, 0xffffffff, 0x00000000 };

    if (m >= 4) {
        for ( ; i < m; i += 4) {
            v128 _x = vld1q_f32(reinterpret_cast<float*>(x + i));
            v128 _u = vdupq_n_f32(reinterpret_cast<float*>(p)[0]);
            v128 _v = vdupq_n_f32(reinterpret_cast<float*>(p)[1]);
            v128 _y = vbslq_f32(_mask, _u, _v);

            for (size_t j = 1; j < n; ++j) {
                _u = vdupq_n_f32(reinterpret_cast<float*>(p + j)[0]);
                _v = vdupq_n_f32(reinterpret_cast<float*>(p + j)[1]);

                v128 _a = vbslq_f32(_mask, _u, _v);

                _y = vcmulq_f32(_y, _x);
                _y = vaddq_f32(_y, _a);
            }

            vst1q_f32(reinterpret_cast<float*>(y + i), _y);
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