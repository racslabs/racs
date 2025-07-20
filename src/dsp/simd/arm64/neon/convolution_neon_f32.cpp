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

#include "../../convolution.hpp"

namespace racs::simd {

#ifdef __ARM_NEON__

void convolve(float* a, float* b, float* c, size_t n, size_t m) {
    size_t i = 0;

    if (n >= 4) {
        for ( ; i < n; i += 4) {
            for (size_t j = 0; j < m; ++j) {
                v128 _a = vld1q_f32(a + i + j);
                v128 _b = vdupq_n_f32(b[j]);
                v128 _c = vld1q_f32(c + i);

                _c = vmlaq_f32(_c, _a, _b);
                vst1q_f32(c + i, _c);
            }
        }
    }

    for ( ; i < n; ++i) {
        for (size_t j = 0; j < m; ++j) {
            c[i] += a[i + j] * b[j];
        }
    }
}

void transposed_convolver(float* a, float* b, float* c, size_t n, size_t m, size_t s) {
    for (size_t i = 0; i < n; ++i) {

        size_t j = 0;
        size_t k = i * s;

        v128 _a = vdupq_n_f32(a[i]);

        if (m >= 4) {
            for ( ; j < m; j += 4) {
                v128 _b = vld1q_f32(b + j);
                v128 _c = vld1q_f32(c + k + j);

                _c = vmlaq_f32(_c, _a, _b);

                vst1q_f32(c + k + j, _c);
            }
        }

        for ( ; j < m; ++j) {
            c[k + j] += a[i] * b[j];
        }
    }
}
#endif

}
