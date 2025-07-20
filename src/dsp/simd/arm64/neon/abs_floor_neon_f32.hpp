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

#ifndef RACS_SIMD_ABS_FLOOR_NEON_F32_HPP
#define RACS_SIMD_ABS_FLOOR_NEON_F32_HPP

#include "../../abs_floor.hpp"

namespace racs::simd {

#ifdef __ARM_NEON__

RACS_SIMD v128 RACS_VECTORCALL vfloorq_f32(v128 _a) {
    v128i _b = vcvtq_s32_f32(_a);
    v128u _mask = vcltq_f32(_a, vdupq_n_f32(0.0f));

    _b = vaddq_s32(_b, _mask);

    return vcvtq_f32_s32(_b);
}

#endif

}

#endif //RACS_SIMD_ABS_FLOOR_NEON_F32_HPP
