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

#ifndef RACS_SIMD_POLYBLEP_NEON_F32_HPP
#define RACS_SIMD_POLYBLEP_NEON_F32_HPP

#include "../../polyblep.hpp"

namespace racs::simd {

#ifdef __ARM_NEON__

RACS_SIMD v128 RACS_VECTORCALL vpolyblepq_f32(v128 _pd, v128 _t) {
    const v128 _v0 = vdupq_n_f32(pi2_rec<float>);
    const v128 _v1 = vdupq_n_f32(1.0f);
    const v128 _v2 = vdupq_n_f32(0);

    v128 _dt = vmulq_f32(_pd, _v0);
    v128u _mask_lt = vcltq_f32(_t, _dt);
    v128u _mask_gt = vcgtq_f32(_t, vsubq_f32(_v1, _dt));

    v128 _lt_t0 = vdivq_f32(_t, _dt);
    v128 _lt_t1 = vmulq_f32(_lt_t0, _lt_t0);
    _lt_t1 = vsubq_f32(vaddq_f32(_lt_t0, _lt_t0), _lt_t1);
    _lt_t1 = vsubq_f32(_lt_t1, _v1);
    _lt_t1 = vbslq_f32(_mask_lt, _lt_t1, _v2);

    v128 _gt_t0 = vdivq_f32(vsubq_f32(_t, _v1), _dt);
    v128 _gt_t1 = vmulq_f32(_gt_t0, _gt_t0);
    _gt_t1 = vaddq_f32(vaddq_f32(_gt_t0, _gt_t0), _gt_t1);
    _gt_t1 = vaddq_f32(_gt_t1, _v1);
    _gt_t1 = vbslq_f32(_mask_gt, _gt_t1, _v2);

    v128u _mask_ne = vmvnq_u32(vceqq_f32(_lt_t1, _v2));
    return vbslq_f32(_mask_ne, _lt_t1, _gt_t1);
}
#endif

}


#endif //RACS_SIMD_POLYBLEP_NEON_F32_HPP
