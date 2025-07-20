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

#ifndef RACS_SIMD_RELU_NEON_F32_HPP
#define RACS_SIMD_RELU_NEON_F32_HPP

#include "../../relu.hpp"

namespace racs::simd {

#ifdef __ARM_NEON__

RACS_SIMD v128 RACS_VECTORCALL vlreluq_f32(v128 _x, float a) {
    const v128 _a = vdupq_n_f32(a);
    const v128 _c0 = vdupq_n_f32(0);
    const v128u _mask = vcltq_f32(_x, _c0);

    v128 _xa = vmulq_f32(_x, _a);

    return vbslq_f32(_mask, _xa, _x);
}

RACS_SIMD v128 RACS_VECTORCALL vreluq_f32(v128 _x) {
    const v128 _c0 = vdupq_n_f32(0);
    const v128u _mask = vcltq_f32(_x, _c0);

    return vbslq_f32(_mask, _c0, _x);
}

#endif

}

#endif //RACS_SIMD_RELU_NEON_F32_HPP
