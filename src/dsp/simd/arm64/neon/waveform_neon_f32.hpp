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

#ifndef RACS_SIMD_WAVEFORM_NEON_F32_HPP
#define RACS_SIMD_WAVEFORM_NEON_F32_HPP

#include "../../waveforms.hpp"

namespace racs::simd {

#ifdef __ARM_NEON__

RACS_SIMD v128 RACS_VECTORCALL vblsawq_f32(v128 _p, v128 _pd) {
    const v128 _v0 = vdupq_n_f32(pi2_rec<float>);
    const v128 _v1 = vdupq_n_f32(2.0f);
    const v128 _v2 = vdupq_n_f32(1.0f);

    v128 _t, _a;

    _t = vmulq_f32(_p, _v0);
    _a = vsubq_f32(vmulq_f32(_t, _v1), _v2);

    return vsubq_f32(_a, vpolyblepq_f32(_pd, _t));
}

RACS_SIMD v128 RACS_VECTORCALL vsquareq_f32(v128 _p) {
    const v128 _v0 = vdupq_n_f32(pi<float>);
    const v128 _v1 = vdupq_n_f32(1.0f);
    const v128 _v2 = vdupq_n_f32(-1.0f);

    v128u _mask = vcltq_f32(_p, _v0);
    return vbslq_f32(_mask, _v2, _v1);
}

RACS_SIMD v128 RACS_VECTORCALL vblsquareq_f32(v128 _p, v128 _pd) {
    const v128 _v0 = vdupq_n_f32(pi2_rec<float>);
    const v128 _v1 = vdupq_n_f32(1.0f);
    const v128 _v2 = vdupq_n_f32(0.5f);

    v128 _t = vmulq_f32(_p, _v0);
    v128 _t0 = vmodq_f32(vaddq_f32(_t, _v2), _v1);

    v128 _a = vsquareq_f32(_p);
    _a = vaddq_f32(vpolyblepq_f32(_pd, _t), _a);

    return vsubq_f32(_a, vpolyblepq_f32(_pd, _t0));
}
#endif
}

#endif //RACS_SIMD_WAVEFORM_NEON_F32_HPP
