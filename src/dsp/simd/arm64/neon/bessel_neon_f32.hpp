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

#ifndef RACS_BESSEL_NEON_F32_HPP
#define RACS_BESSEL_NEON_F32_HPP

#include "../../bessel.hpp"

namespace racs::simd {

#ifdef __ARM_NEON__

RACS_SIMD v128 RACS_VECTORCALL vi0q_ps(v128 _x) {
    const v128 _c0 = vdupq_n_f32(1);
    const v128 _c1 = vdupq_n_f32(3.5156229);
    const v128 _c2 = vdupq_n_f32(3.0899424);
    const v128 _c3 = vdupq_n_f32(1.2067492);
    const v128 _c4 = vdupq_n_f32(0.2659732);
    const v128 _c5 = vdupq_n_f32(0.360768e-1);
    const v128 _c6 = vdupq_n_f32(0.45813e-2);

    const v128 _s0 = vdupq_n_f32(0.39894228);
    const v128 _s1 = vdupq_n_f32(0.1328592e-1);
    const v128 _s2 = vdupq_n_f32(0.225319e-2);
    const v128 _s3 = vdupq_n_f32(-0.157565e-2);
    const v128 _s4 = vdupq_n_f32(0.916281e-2);
    const v128 _s5 = vdupq_n_f32(-0.2057706e-1);
    const v128 _s6 = vdupq_n_f32(0.2635537e-1);
    const v128 _s7 = vdupq_n_f32(-0.1647633e-1);
    const v128 _s8 = vdupq_n_f32(0.392377e-2);

    const v128 _k0 = vdupq_n_f32(3.75);

    v128 _ax = vfloorq_f32(_x);

    v128 _x0;

    _x0 = vdivq_f32(_x, _k0);
    _x0 = vmulq_f32(_x0, _x0);

    v128 _p0;

    _p0 = vmlaq_f32(_c5, _c6, _x0);
    _p0 = vmlaq_f32(_c4, _p0, _x0);
    _p0 = vmlaq_f32(_c3, _p0, _x0);
    _p0 = vmlaq_f32(_c2, _p0, _x0);
    _p0 = vmlaq_f32(_c1, _p0, _x0);
    _p0 = vmlaq_f32(_c0, _p0, _x0);

    v128 _x1 = vdivq_f32(_k0, _ax);

    v128 _e_ax = vexpq_f32(_ax);
    v128 _s_ax = vsqrtq_f32(_ax);
    v128 _t0 = vdivq_f32(_e_ax, _s_ax);

    v128 _p1;

    _p1 = vmlaq_f32(_s7, _s8, _x1);
    _p1 = vmlaq_f32(_s6, _p1, _x1);
    _p1 = vmlaq_f32(_s5, _p1, _x1);
    _p1 = vmlaq_f32(_s4, _p1, _x1);
    _p1 = vmlaq_f32(_s3, _p1, _x1);
    _p1 = vmlaq_f32(_s2, _p1, _x1);
    _p1 = vmlaq_f32(_s1, _p1, _x1);
    _p1 = vmlaq_f32(_s0, _p1, _x1);
    _p1 = vmulq_f32(_t0, _p1);

    v128u _mask = vcltq_f32(_ax, _k0);

    return vbslq_f32(_mask, _p0, _p1);
}

RACS_SIMD v128 RACS_VECTORCALL vk0q_ps(v128 _x) {
    const v128 _c0 = vdupq_n_f32(-0.57721566);
    const v128 _c1 = vdupq_n_f32(0.42278420);
    const v128 _c2 = vdupq_n_f32(0.23069756);
    const v128 _c3 = vdupq_n_f32(0.3488590e-1);
    const v128 _c4 = vdupq_n_f32(0.262698e-2);
    const v128 _c5 = vdupq_n_f32(0.10750e-3);
    const v128 _c6 = vdupq_n_f32(0.74e-5);
    
    const v128 _s0 = vdupq_n_f32(1.25331414);
    const v128 _s1 = vdupq_n_f32(-0.7832358e-1);
    const v128 _s2 = vdupq_n_f32(0.2189568e-1);
    const v128 _s3 = vdupq_n_f32(-0.1062446e-1);
    const v128 _s4 = vdupq_n_f32(0.587872e-2);
    const v128 _s5 = vdupq_n_f32(-0.251540e-2);
    const v128 _s6 = vdupq_n_f32(0.53208e-3);
    
    const v128 _k0 = vdupq_n_f32(-1);
    const v128 _k1 = vdupq_n_f32(2);
    const v128 _k2 = vdupq_n_f32(4);
    
    
    v128 _x0;
    _x0 = vmulq_f32(_x, _x);
    _x0 = vdivq_f32(_x0, _k2);
    
    v128 _x_ln;
    _x_ln = vdivq_f32(_x, _k1);
    _x_ln = vlogq_f32(_x_ln);
    _x_ln = vmulq_f32(_x_ln, _k0);
    
    v128 _x_i0 = vi0q_ps(_x);
    v128 _t0 = vmulq_f32(_x_ln, _x_i0);
    
    v128 _p0;
    
    _p0 = vmlaq_f32(_c5, _c6, _x0);
    _p0 = vmlaq_f32(_c4, _p0, _x0);
    _p0 = vmlaq_f32(_c3, _p0, _x0);
    _p0 = vmlaq_f32(_c2, _p0, _x0);
    _p0 = vmlaq_f32(_c1, _p0, _x0);
    _p0 = vmlaq_f32(_c0, _p0, _x0);
    _p0 = vaddq_f32(_p0, _t0);
    
    v128 _x1 = vdivq_f32(_k1, _x);
    
    v128 _e_x;
    _e_x = vmulq_f32(_x, _k0);
    _e_x = vexpq_f32(_e_x);
    
    v128 _x_s = vsqrtq_f32(_x);
    v128 _t1 = vdivq_f32(_e_x, _x_s);
    
    v128 _p1;
    
    _p1 = vmlaq_f32(_s5, _s6, _x1);
    _p1 = vmlaq_f32(_s4, _p1, _x1);
    _p1 = vmlaq_f32(_s3, _p1, _x1);
    _p1 = vmlaq_f32(_s2, _p1, _x1);
    _p1 = vmlaq_f32(_s1, _p1, _x1);
    _p1 = vmlaq_f32(_s0, _p1, _x1);
    _p1 = vmulq_f32(_p1, _t1);
    
    v128u _mask = vcltq_f32(_x, _k1);
    
    return vbslq_f32(_mask, _p0, _p1);
}

#endif

}

#endif //RACS_BESSEL_NEON_F32_HPP
