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

#include "../../dct.hpp"

namespace racs::simd {

#ifdef __ARM_NEON__

void dct_even_odd(float* x, float* y, size_t n) {
    for (int i = 0; i < n; i += 4) {
        auto m = i >> 0x1;

        y[m] = x[m];
        y[m + 1] = x[m + 2];
        y[n - m - 1] = x[m + 3];
        y[n - m - 2] = x[m + 1];
    }
}

void idct_even_odd(float* x, float* y, size_t n) {
    for (int i = 0; i < n / 2; i += 2) {
        auto m = i << 0x1;

        y[m] = x[i];
        y[m + 1] = x[n - i - 1];
        y[m + 2] = x[i + 1];
        y[m + 3] = x[n - i - 2];
    }
}
#endif

}
