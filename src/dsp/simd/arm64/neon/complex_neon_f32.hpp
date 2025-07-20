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

#ifndef RACS_SIMD_COMPLEX_NEON_F32_HPP
#define RACS_SIMD_COMPLEX_NEON_F32_HPP

#include "../../complex.hpp"

namespace racs::simd {

#ifdef __ARM_NEON__

RACS_SIMD v128 RACS_VECTORCALL vcexpq_f32(v128 _x) {
    v128 _re = vcosq_f32(_x);
    v128 _im = vsinq_f32(_x);

    return vtrn1q_f32(_re, _im);
}

RACS_SIMD v128 RACS_VECTORCALL vcsquareq_f32(v128 _x) {
    return vcmulq_f32(_x, _x);
}

RACS_SIMD v128 RACS_VECTORCALL vcconjq_f32(v128 _x) {
    const v128 _c0 = { 1, -1, 1, -1 };
    return vmulq_f32(_x, _c0);
}

RACS_SIMD v128 RACS_VECTORCALL vr2cq_f32(v128 _r) {
    return vzip1q_f32(_r, vdupq_n_f32(0));
}

RACS_SIMD v64 RACS_VECTORCALL vrealq_f32(v128 _x) {
    v128 _y = vuzp1q_f32(_x, vdupq_n_f32(0));
    return vget_low_f32(_y);
}

RACS_SIMD v64 RACS_VECTORCALL vimagq_f32(v128 _x) {
    v128 _y = vuzp2q_f32(_x, vdupq_n_f32(0));
    return vget_low_f32(_y);
}

RACS_SIMD v64 RACS_VECTORCALL vmagq_f32(v128 _x) {
    const v128 _c0 = vdupq_n_f32(0);

    v128 _a = vuzp1q_f32(_x, _c0);
    v128 _b = vuzp2q_f32(_x, _c0);

    _a = vmulq_f32(_a, _a);
    _b = vmulq_f32(_b, _b);

    v128 _c = vaddq_f32(_a, _b);

    return vget_low_f32(vsqrtq_f32(_c));
}

RACS_SIMD v128 RACS_VECTORCALL vcexpzq_f32(v128 _x) {
    v128 _re = vtrn1q_f32(_x, _x);
    v128 _im = vtrn2q_f32(_x, _x);

    v128 _a = vexpq_f32(_re);
    v128 _b = vcexpq_f32(_im);

    return vmulq_f32(_a, _b);
}

RACS_SIMD v128 RACS_VECTORCALL vcsinhq_f32(v128 _x) {
    const v128 _c0 = vdupq_n_f32(-1);
    const v128 _c1 = vdupq_n_f32(2);

    v128 _a = vcexpzq_f32(_x);
    v128 _b = vcexpzq_f32(vmulq_f32(_x, _c0));
    v128 _c = vsubq_f32(_a, _b);

    return vdivq_f32(_c, _c1);
}
#endif

}

#endif //RACS_SIMD_COMPLEX_NEON_F32_HPP
