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

#ifndef RACS_SIMD_ELU_NEON_F32_HPP
#define RACS_SIMD_ELU_NEON_F32_HPP

#include "../../elu.hpp"

namespace racs::simd {

#ifdef __ARM_NEON__

RACS_SIMD v128 RACS_VECTORCALL veluq_f32(v128 _x, float a) {
    const v128 _c0 = vdupq_n_f32(0);
    const v128 _c1 = vdupq_n_f32(1);
    const v128 _a = vdupq_n_f32(a);

    v128u _mask = vcltq_f32(_x, _c0);
    v128  _xn = vexpq_f32(_x);

    _xn = vsubq_f32(_xn, _c1);
    _xn = vmulq_f32(_a, _xn);

    return vbslq_f32(_mask, _xn, _x);
}
#endif

}



#endif //RACS_SIMD_ELU_NEON_F32_HPP
