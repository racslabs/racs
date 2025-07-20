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

#ifndef RACS_SIMD_EXPONENTIAL_NEON_F32_HPP
#define RACS_SIMD_EXPONENTIAL_NEON_F32_HPP

#include "../../exponential.hpp"

namespace racs::simd {

#ifdef __ARM_NEON__

RACS_SIMD v128 RACS_VECTORCALL vexpq_f32(v128 _x) {
    const v128 _c0 = vdupq_n_f32(1.44269504089);
    const v128 _c1 = vdupq_n_f32(8388608);
    const v128 _c2 = vdupq_n_f32(1065353216);

    const v128 _s1 = vdupq_n_f32(3.06852819440055e-1);
    const v128 _s2 = vdupq_n_f32(-2.40226506959101e-1);
    const v128 _s3 = vdupq_n_f32(-5.57129652016652e-2);
    const v128 _s4 = vdupq_n_f32(-9.01146535969578e-3);
    const v128 _s5 = vdupq_n_f32(-1.90188191959304e-3);

    union { v128 _f; v128i _i; } u{};

    v128 _y = vmulq_f32(_x, _c0);
    v128 _yf = vsubq_f32(_y, vfloorq_f32(_y));

    v128 _d = vmlaq_f32(_s4, _yf, _s5);
    _d = vmlaq_f32(_s3, _yf, _d);
    _d = vmlaq_f32(_s2, _yf, _d);
    _d = vmlaq_f32(_s1, _yf, _d);
    _d = vmulq_f32(_yf, _d);

    v128 _yd = vsubq_f32(_y, _d);
    v128 _z = vmlaq_f32(_c2, _yd, _c1);

    u._i = vcvtq_s32_f32(_z);

    return u._f;
}

RACS_SIMD v128 RACS_VECTORCALL vpowq_f32(v128 _b, v128 _x) {
    v128 _a = vmulq_f32(_x, vlogq_f32(_b));
    return vexpq_f32(_a);
}
#endif

}

#endif //RACS_SIMD_EXPONENTIAL_NEON_F32_HPP
