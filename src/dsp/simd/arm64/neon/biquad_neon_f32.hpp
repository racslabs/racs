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

#ifndef RACS_SIMD_BIQUAD_NEON_F32_HPP
#define RACS_SIMD_BIQUAD_NEON_F32_HPP

#include "../../biquad.hpp"

namespace racs::simd {

#ifdef __ARM_NEON__

struct Biquad_config_f32 {
    v128 c_[10];
};

RACS_SIMD v128 RACS_VECTORCALL vbiquadfirq_f32(v128 _w0,
                                                 v128 _w1,
                                                 v128 _w2,
                                                 v128 _b0,
                                                 v128 _b1,
                                                 v128 _b2) {
    v128 _r;

    _r = vmulq_f32(_b0, _w0);
    _r = vmlaq_f32(_r, _b1, _w1);
    _r = vmlaq_f32(_r, _b2, _w2);
    return _r;
}

RACS_SIMD v128 RACS_VECTORCALL vbiquadiirq_f32(v128 _x,
                                                 v128 _xk,
                                                 v128 _xk1,
                                                 v128 _xk2,
                                                 v128 _wk1,
                                                 v128 _wk2,
                                                 v128* _c) {
    v128 _r;

    _r = vmlaq_f32(_x, _c[0], _wk1);
    _r = vmlaq_f32(_r, _c[1], _wk2);
    _r = vmlaq_f32(_r, _c[2], _xk);
    _r = vmlaq_f32(_r, _c[3], _wk1);
    _r = vmlaq_f32( _r, _c[4], _xk1);
    _r = vmlaq_f32(_r, _c[5], _xk);
    _r = vmlaq_f32(_r, _c[6], _wk2);
    _r = vmlaq_f32(_r, _c[7], _xk2);
    _r = vmlaq_f32(_r, _c[8], _xk1);

    return vmlaq_f32(_r, _c[9], _wk1);
}
#endif

}


#endif //RACS_SIMD_BIQUAD_NEON_F32_HPP
