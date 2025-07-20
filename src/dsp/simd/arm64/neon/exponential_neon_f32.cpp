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

#include "exponential_neon_f32.hpp"

namespace racs::simd {

#ifdef __ARM_NEON__

void exp(float* a, float* b, size_t n)  {
    size_t i = 0;

    if (n >= 4) {
        for ( ; i < n; i += 4) {
            v128 _a = vld1q_f32(a + i);
            vst1q_f32(b + i, vexpq_f32(_a));
        }
    }

    for ( ; i < n; ++i) {
        b[i] = expf(a[i]);
    }
}

void power(float* b, float* x, float* y, size_t n) {
    size_t i = 0;

    if (n >= 4) {
        for ( ; i < n; i += 4) {
            v128 _b = vld1q_f32(b + i);
            v128 _x = vld1q_f32(x + i);
            vst1q_f32(y + i, vpowq_f32(_b, _x));
        }
    }

    for ( ; i < n; ++i) {
        y[i] = powf(b[i], x[i]);
    }
}
#endif

}
