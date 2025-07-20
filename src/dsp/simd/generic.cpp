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

#include "generic.hpp"

namespace racs::simd {

float mod(float a, float b) {
    auto c = static_cast<int>(a / b);
    return a - b * c;
}

void planar(const float *q0, float *q1, size_t n) {
    RACS_UNROLL(8)
    for (int i = 0; i < n / 2; ++i) {
        q1[i] = q0[i * 2];
        q1[i + n / 2] = q0[i * 2 + 1];
    }
}

}
