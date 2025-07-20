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

#include "waveform_neon_f32.hpp"

namespace racs::simd {

#ifdef __ARM_NEON__

void sawtooth(float *p, float *pd, float *a, size_t n) {
    size_t i = 0;

    if (n >= 4) {
        for (; i < n; i += 4) {
            v128 _p = vld1q_f32(p + i);
            v128 _pd = vld1q_f32(pd + i);
            vst1q_f32(a + i, vblsawq_f32(_p, _pd));
        }
    }

    for ( ; i < n; ++i) {
        a[i] = sawtooth(p[i], pd[i]);
    }
}

void square(float *p, float *pd, float *a, size_t n) {
    size_t i = 0;

    if (n >= 4) {
        for (; i < n; i += 4) {
            v128 _p = vld1q_f32(p + i);
            v128 _pd = vld1q_f32(pd + i);
            vst1q_f32(a + i, vblsquareq_f32(_p, _pd));
        }
    }

    for ( ; i < n; ++i) {
        a[i] = square(p[i], pd[i]);
    }
}
#endif

}