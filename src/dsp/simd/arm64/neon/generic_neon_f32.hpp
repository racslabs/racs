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

#ifndef RACS_SIMD_GENERIC_NEON_F32_HPP
#define RACS_SIMD_GENERIC_NEON_F32_HPP

#include "../../generic.hpp"

namespace racs::simd {

#ifdef __ARM_NEON__

RACS_SIMD v128 RACS_VECTORCALL vpsubq_f32(v128 _a, v128 _b) {
    const v128 _neg = { 1, -1, 1, -1 };

    _a = vmulq_f32(_a, _neg);
    _b = vmulq_f32(_b, _neg);

    return vpaddq_f32(_a, _b);
}

RACS_SIMD v128 RACS_VECTORCALL vcmulq_f32(v128 _a, v128 _b) {
    const v128 _neg = { 1, -1, 1, -1 };

    v128 _c = vmulq_f32(_a, _b);
    v128 _bt = vtrn1q_f32(_b, _b);

    _b = vtrn2q_f32(_b, _bt);
    _b = vmulq_f32(_b,  _neg);

    v128 _d = vmulq_f32(_a, _b);
    _c = vpsubq_f32(_c, _d);

    v128 _ct = vtrn2q_f32(_c, _c);
    return vuzp1q_f32(_c, _ct);
}

RACS_SIMD v128 RACS_VECTORCALL vcdivq_f32(v128 _a, v128 _b) {
    const v128 _neg = { 1, -1, 1, -1 };

    v128 _bc = vmulq_f32(_b, _neg);
    v128 _n = vcmulq_f32(_a, _bc);

    v128 _d = vcmulq_f32(_b, _bc);
    _d = vtrn1q_f32(_d, _d);

    return vdivq_f32(_n, _d);
}

RACS_SIMD v128 RACS_VECTORCALL vmodq_f32(v128 _a, v128 _b) {
    v128 _c = vdivq_f32(_a, _b);
    _c = vfloorq_f32(_c);
    v128 _d = vmulq_f32(_c, _b);

    return vsubq_f32(_a, _d);
}

RACS_SIMD v128 RACS_VECTORCALL vrevq_f32(v128 _a) {
    _a = vrev64q_f32(_a);

    v64 _hi = vget_high_f32(_a);
    v64 _lo = vget_low_f32(_a);

    return vcombine_f32(_hi, _lo);
}

RACS_SIMD v128 RACS_VECTORCALL vcrevq_f32(v128 _a) {
    v128 _b = vtrn2q_f32(_a, _a);
    _a = vtrn1q_f32(_b, _a);

    return vrevq_f32(_a);
}
#endif

}
#endif //RACS_SIMD_GENERIC_NEON_F32_HPP
