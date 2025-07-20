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

#ifndef RACS_SIMD_TRIGONOMETRIC_NEON_F32_HPP
#define RACS_SIMD_TRIGONOMETRIC_NEON_F32_HPP

#include "../../trigonometric.hpp"

namespace racs::simd {

#ifdef __ARM_NEON__

RACS_SIMD v128 RACS_VECTORCALL vsinq_f32(v128 _x) {
    const v128 _a0 = vdupq_n_f32(1);
    const v128 _a1 = vdupq_n_f32(-0.166666666666666666666666666666666666666666666666666666666666666);
    const v128 _a2 = vdupq_n_f32(0.0083333333333333333333333333333333333333333333333333333333333333);
    const v128 _a3 = vdupq_n_f32(-0.000198412698412698412698412698412698412698412698412698412698412);
    const v128 _a4 = vdupq_n_f32(2.7557319223985890652557319223985890652557319223985890652557e-6);
    const v128 _a5 = vdupq_n_f32(-2.505210838544171877505210838544171877505210838544171877505e-8);
    const v128 _a6 = vdupq_n_f32(1.6059043836821614599392377170154947932725710503488281266059e-10);
    const v128 _a7 = vdupq_n_f32(-7.64716373181981647590113198578807044415510023975632441240e-13);
    const v128 _a8 = vdupq_n_f32(2.8114572543455207631989455830103200162334927352045310339739e-15);

    const v128 _c0 = vdupq_n_f32(0.1591549430918953357688837633725143620344596457404564487476673440);
    const v128 _c1 = vdupq_n_f32(6.2831853071795864769252867665590057683943387987502116419498891846);
    const v128 _c2 = vdupq_n_f32(3.1415926535897932384626433832795028841971693993751058209749445923);

    v128 _xf = vfloorq_f32(vmulq_f32(_x, _c0));
    _x = vsubq_f32(_x, vmulq_f32(_xf, _c1));
    _x = vsubq_f32(_c2, _x);

    v128 _x2 = vmulq_f32(_x, _x);
    v128 _y = vmlaq_f32(_a7, _x2, _a8);
    _y = vmlaq_f32(_a6, _x2, _y);
    _y = vmlaq_f32(_a5, _x2, _y);
    _y = vmlaq_f32(_a4, _x2, _y);
    _y = vmlaq_f32(_a3, _x2, _y);
    _y = vmlaq_f32(_a2, _x2, _y);
    _y = vmlaq_f32(_a1, _x2, _y);
    _y = vmlaq_f32(_a0, _x2, _y);

    return vmulq_f32(_x, _y);

}

RACS_SIMD v128 RACS_VECTORCALL vsincq_f32(v128 _x) {
    const v128 _pi = vdupq_n_f32(3.1415926535897932384626433832795028841971693993751058209749445923);
    const v128 _c0 = vdupq_n_f32(0);
    const v128 _c1 = vdupq_n_f32(1);

    v128u _mask = vceqq_f32(_x, _c0);

    _x = vmulq_f32(_x, _pi);

    v128 _n = vsinq_f32(_x);
    _n = vdivq_f32(_n, _x);

    return vbslq_f32(_mask, _n, _c1);
}

RACS_SIMD v128 RACS_VECTORCALL vcosq_f32(v128 _x) {
    const v128 _c0 = vdupq_n_f32(1.5707963267948966192313216916397514420985846996875529104874722961);
    return vsinq_f32(vsubq_f32(_c0, _x));
}

RACS_SIMD v128 RACS_VECTORCALL vtanq_f32(v128 _a) {
    v128 _b = vsinq_f32(_a);
    v128 _c = vcosq_f32(_a);
    return vdivq_f32(_b, _c);
}

RACS_SIMD v128 RACS_VECTORCALL vcotq_f32(v128 _a) {
    v128 _b = vcosq_f32(_a);
    v128 _c = vsinq_f32(_a);
    return vdivq_f32(_b, _c);
}

RACS_SIMD v128 RACS_VECTORCALL vsecq_f32(v128 _a) {
    const v128 _v = vdupq_n_f32(1.0);
    return vdivq_f32(_v, vcosq_f32(_a));
}

RACS_SIMD v128 RACS_VECTORCALL vcscq_f32(v128 _a) {
    const v128 _v = vdupq_n_f32(1.0);
    return vdivq_f32(_v, vsinq_f32(_a));
}
#endif

}


#endif //RACS_SIMD_TRIGONOMETRIC_NEON_F32_HPP
