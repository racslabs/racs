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

#include "normal_neon_f32.hpp"

namespace racs::simd {

#ifdef __ARM_NEON__

void normal(float* x, float* y, float mu, float sigma, size_t n) {
    const v128 _mu = vdupq_n_f32(mu);
    const v128 _sigma = vdupq_n_f32(sigma);
    const v128 _c0 = vdupq_n_f32(0.3989422804 * (1 / sigma));

    size_t i = 0;

    if (n >= 4) {
        for (; i < n; i += 4) {
            v128 _x = vld1q_f32(x + i);
            v128 _y = vnormalq_f32(_x, _mu, _sigma);
            _y = vmulq_f32(_c0, _y);
            vst1q_f32(y + i, _y);
        }
    }

    for ( ; i < n; ++i) {
        y[i] = normal(x[i], mu, sigma);
    }
}
#endif

}
