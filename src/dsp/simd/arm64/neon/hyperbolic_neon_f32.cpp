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

#include "hyperbolic_neon_f32.hpp"

namespace racs::simd {

#ifdef __ARM_NEON__

void sinh(float *a, float *b, size_t n) {
    size_t i = 0;

    if (n >= 4) {
        for (; i < n; i += 4) {
            v128 _a = vld1q_f32(a + i);
            vst1q_f32(b + i, vsinhq_f32(_a));
        }
    }

    for (; i < n; ++i) {
        b[i] = sinhf(a[i]);
    }
}

void cosh(float *a, float *b, size_t n) {
    size_t i = 0;

    if (n >= 4) {
        for (; i < n; i += 4) {
            v128 _a = vld1q_f32(a + i);
            vst1q_f32(b + i, vcoshq_f32(_a));
        }
    }

    for (; i < n; ++i) {
        b[i] = coshf(a[i]);
    }
}

void tanh(float *a, float *b, size_t n) {
    size_t i = 0;

    if (n >= 4) {
        for (; i < n; i += 4) {
            v128 _a = vld1q_f32(a + i);
            vst1q_f32(b + i, vtanhq_f32(_a));
        }
    }

    for (; i < n; ++i) {
        b[i] = tanhf(a[i]);
    }
}
#endif

}
