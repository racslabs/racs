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

#include "waveforms.hpp"

namespace racs::simd {

float sawtooth(float p, float pd) {
    float t = p * pi2_rec<float>;
    float a = t * 2 - 1;

    return a - polyblep(pd, t);
}

float square(float p, float pd) {
    float t = p * pi2_rec<float>;
    float t0 = mod(t + 0.5f, 1);

    float a = p < pi<float> ? 1 : -1;
    a += polyblep(pd, t);

    return a - polyblep(pd, t0);
}

void leaky_integrator(float* p, float* pd, float* a, size_t n) {
    a[0] = pd[0] * p[0];

    RACS_UNROLL(4)
    for (int i = 1; i < n; ++i) {
        a[i] = pd[i] * p[i] + (1 - pd[i]) * a[i - 1];
    }
}

}