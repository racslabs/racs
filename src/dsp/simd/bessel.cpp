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

#include "bessel.hpp"

namespace racs::simd {

float i0(float x) {
    float ax, p;

    if ((ax = fabsf(x)) < 3.75f) {
        p = x / 3.75f;
        p *= p;
        return 1.0f + p * (3.5156229f + p * (3.0899424f + p * (1.2067492f + p * (0.2659732f + p *
                (0.0360768f + p * 0.0045813f)))));
    }

    p = 3.75f / ax;
    return (expf(ax) / sqrtf(ax)) * (0.39894228f + p * (0.01328592f + p * (0.00225319f + p * (-0.00157565f + p *
            (0.00916281f + p * (-0.02057706f + p * (0.02635537f + p * (-0.01647633f + p * 0.00392377f))))))));
}

float k0(float x) {
    float p;

    if (x <= 2.0f) {
        p = x * x / 4.0f;
        return (-logf(x / 2.0f) * i0(x)) + (-0.57721566f + p * (0.42278420f + p * (0.23069756f + p * (0.03488590f +
                p * (0.00262698f + p * (0.00010750f + p * 0.00000740f))))));
    }

    p = 2.0f / x;
    return (expf(-x) / sqrtf(x)) * (1.25331414f + p * (-0.07832358f + p * (0.02189568f + p * (-0.01062446f + p *
            (0.00587872f + p * (-0.00251540f + p * 0.00053208f))))));
}


}