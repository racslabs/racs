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

#include "../../envelope.hpp"

namespace racs::simd {

#ifdef __ARM_NEON__
void envelope(float* x, float level, float a0, size_t n) {
    alignas(32) float c[4][4] = {
            { 1, 2, 3, 4 },
            { 6, 3, 1, 0 },
            { 4, 1, 0, 0 },
            { 1, 0, 0, 0 }
    };

    const v128 _a0 = vdupq_n_f32(a0);
    const v128 _a1 = vmulq_f32(_a0, _a0);
    const v128 _a2 = vmulq_f32(_a1, _a0);
    const v128 _a3 = vmulq_f32(_a2, _a0);

    const v128 _c0 = vmulq_f32(_a0, vld1q_f32(c[0]));
    const v128 _c1 = vmulq_f32(_a1, vld1q_f32(c[1]));
    const v128 _c2 = vmulq_f32(_a2, vld1q_f32(c[2]));
    const v128 _c3 = vmulq_f32(_a3, vld1q_f32(c[3]));

    v128 _x = vdupq_n_f32(level);
    v128 _y = vmlaq_f32(_x, _c0, _x);

    if (n >= 4) {
        _y = vmlaq_f32(_y, _x, _c1);
        _y = vmlaq_f32(_y, _x, _c2);
        _y = vmlaq_f32(_y, _x, _c3);

        vst1q_f32(x, _y);
    }

    size_t i = 4;

    if (n >= 8) {
        for ( ;i < n; i += 4) {
            _x = vdupq_n_f32(x[i - 1]);
            _y = vmlaq_f32(_x, _x, _c0);
            _y = vmlaq_f32(_y, _x, _c1);
            _y = vmlaq_f32(_y, _x, _c2);
            _y = vmlaq_f32(_y, _x, _c3);

            vst1q_f32(x + i, _y);
        }
    }

    for ( ; i < n ; ++i) {
        x[i] += x[i - 1] * level;
    }
}
#endif

}
