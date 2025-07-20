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

#ifndef RACS_SIMD_MIN_MAX_NEON_F32_HPP
#define RACS_SIMD_MIN_MAX_NEON_F32_HPP

#include "../../min_max.hpp"

namespace racs::simd {

#ifdef __ARM_NEON__

RACS_SIMD float RACS_VECTORCALL vrminq_f32(v128 _x) {
    v64 _lo = vget_low_f32(_x);
    v64 _hi = vget_high_f32(_x);

    v64 _a = vmin_f32(_lo, _hi);
    v64 _b = vrev64_f32(_a);
    v64 _c = vmin_f32(_a, _b);

    return _c[0];
}

RACS_SIMD float RACS_VECTORCALL vrmaxq_f32(v128 _x) {
    v64 _lo = vget_low_f32(_x);
    v64 _hi = vget_high_f32(_x);

    v64 _a = vmax_f32(_lo, _hi);
    v64 _b = vrev64_f32(_a);
    v64 _c = vmax_f32(_a, _b);

    return _c[0];
}
#endif

}

#endif //RACS_SIMD_MIN_MAX_NEON_F32_HPP
