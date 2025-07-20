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

#include "../../comb.hpp"

namespace racs::simd {

#ifdef __ARM_NEON__

void ffcomb(float* a, float* ap, float* b, float g, size_t n) {
    size_t i = 0;

    const v128 _g = vdupq_n_f32(g);

    if (n >= 4) {
        for ( ; i < n; i += 4) {
            v128 _a = vld1q_f32(a + i);
            v128 _ap = vld1q_f32(ap + i);
            v128 _b = vmlaq_f32(_a, _ap, _g);

            vst1q_f32(b + i, _b);
        }
    }

    for ( ; i < n; ++i) {
        b[i] = a[i] + g * ap[i];
    }
}

void fbcomb(float* a, float* b, float g, size_t n, size_t m) {
    const v128 _g = vdupq_n_f32(g);

    size_t i = 0;

    if (m >= 4) {
        for ( ; i < m; i += 4) {
            v128 _a = vld1q_f32(a + i);
            vst1q_f32(b + i, _a);
        }
    }

    for ( ; i < m; ++i) {
        b[i] = a[i];
    }

    if (n - m >= 4 && m >= 4) {
        for ( ; i < n; i += 4) {
            v128 _a = vld1q_f32(a + i);
            v128 _b = vld1q_f32(b + i - m);
            _b = vmlaq_f32(_a, _g, _b);
            vst1q_f32(b + i, _b);
        }
    }

    for ( ; i < n; ++i) {
        b[i] = a[i] - g * b[i - m];
    }
}

void apcomb(float* a, float* ap, float* b, float g, size_t n, size_t m) {
    size_t i = 0;

    v128 _g = vdupq_n_f32(g * 0.4f);

    // first pass
    if (n >= 4) {
        for ( ; i < n; i += 4) {
            v128 _a = vld1q_f32(a + i);
            v128 _ap = vld1q_f32(ap + i);
            v128 _b = vmlaq_f32(_ap, _g, _a);

            vst1q_f32(b + i, _b);
        }
    }

    for ( ; i < n; ++i) {
        b[i] = g * a[i] + ap[i];
    }

    //second pass
    i = m;

    if (n - m >= 4 && m >= 4) {
        for ( ; i < n; i += 4) {
            v128 _a = vld1q_f32(b + i);
            v128 _b = vld1q_f32(b + i - m);
            _g = vmulq_n_f32(_g, -1);
            _b = vmlaq_f32(_a, _g, _b);
            vst1q_f32(b + i, _b);
        }
    }

    for ( ; i < n; ++i) {
        b[i] = b[i] - g * b[i - m];
    }
}
#endif

}
