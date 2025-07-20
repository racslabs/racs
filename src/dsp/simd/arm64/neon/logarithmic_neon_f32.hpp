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

#ifndef RACS_SIMD_LOGARITHMIC_NEON_F32_HPP
#define RACS_SIMD_LOGARITHMIC_NEON_F32_HPP

#include "../../logarithmic.hpp"

namespace racs::simd {

#ifdef __ARM_NEON__

RACS_SIMD v128 RACS_VECTORCALL vlogq_f32(v128 _x) {
    const v128i _c0 = vdupq_n_s32(127);
    const v128i _c1 = vdupq_n_s32(8388607);
    const v128i _c2 = vdupq_n_s32(1065353216);
    const v128 _c3 = vdupq_n_f32(0.6931471806);

    const v128 _s1 = vdupq_n_f32(-1.74178);
    const v128 _s2 = vdupq_n_f32(2.82117);
    const v128 _s3 = vdupq_n_f32(-1.46994);
    const v128 _s4 = vdupq_n_f32(0.447178);
    const v128 _s5 = vdupq_n_f32(-0.0565717);

    union {v128u _i; v128 _f; } u{._f = _x };

    v128i _bx = u._i;
    v128i _ex = vshrq_n_s32(_bx, 0x17);
    v128i _t = vsubq_s32(_ex, _c0);

    _bx = vandq_s32(_bx, _c1);
    _bx = vorrq_s32(_bx, _c2);

    u._i = _bx;
    _x =  u._f;

    v128 _d = vmlaq_f32(_s4, _x, _s5);
    _d = vmlaq_f32(_s3, _x, _d);
    _d = vmlaq_f32(_s2, _x, _d);
    _d = vmlaq_f32(_s1, _x, _d);

    v128 _t0 = vcvtq_f32_s32(_t);

    return vmlaq_f32(_d, _t0, _c3);
}

RACS_SIMD v128 RACS_VECTORCALL vlog2_f32(v128 _x) {
    const v128 _c0 = vdupq_n_f32(1.4426950409);
    return vmulq_f32(vlogq_f32(_x), _c0);
}

RACS_SIMD v128 RACS_VECTORCALL vlog10_f32(v128 _x) {
    const v128 _c0 = vdupq_n_f32(0.4342944819);
    return vmulq_f32(vlogq_f32(_x), _c0);
}
#endif

}

#endif //RACS_SIMD_LOGARITHMIC_NEON_F32_HPP
