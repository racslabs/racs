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

#ifndef RACS_SIMD_POLYBLEP_AVX_F32_HPP
#define RACS_SIMD_POLYBLEP_AVX_F32_HPP

#include "../../polyblep.hpp"

namespace racs::simd {

#ifdef __AVX2__

RACS_SIMD __m256 RACS_VECTORCALL _mm256_polyblep_ps(__m256 _pd, __m256 _t) {
    const __m256 _v0 = _mm256_set1_ps(pi2_rec<float>);
    const __m256 _v1 = _mm256_set1_ps(1.0f);
    const __m256 _v2 = _mm256_setzero_ps();

    __m256 _dt = _mm256_mul_ps(_pd, _v0);
    __m256 _mask_lt = _mm256_cmp_ps(_t, _dt, _CMP_LT_OS);
    __m256 _mask_gt = _mm256_cmp_ps(_t, _mm256_sub_ps(_v1, _dt), _CMP_GT_OQ);

    __m256 _lt_t0 = _mm256_div_ps(_t, _dt);
    __m256 _lt_t1 = _mm256_mul_ps(_lt_t0, _lt_t0);
    _lt_t1 = _mm256_sub_ps(_mm256_add_ps(_lt_t0, _lt_t0), _lt_t1);
    _lt_t1 = _mm256_sub_ps(_lt_t1, _v1);
    _lt_t1 = _mm256_blendv_ps(_v2, _lt_t1, _mask_lt);

    __m256 _gt_t0 = _mm256_div_ps(_mm256_sub_ps(_t, _v1), _dt);
    __m256 _gt_t1 = _mm256_mul_ps(_gt_t0, _gt_t0);
    _gt_t1 = _mm256_add_ps(_mm256_add_ps(_gt_t0, _gt_t0), _gt_t1);
    _gt_t1 = _mm256_add_ps(_gt_t1, _v1);
    _gt_t1 = _mm256_blendv_ps(_v2, _gt_t1, _mask_gt);

    __m256 _mask = _mm256_cmp_ps(_lt_t1, _v2, _CMP_NEQ_OQ);
    return _mm256_blendv_ps(_gt_t1, _lt_t1, _mask);
}
#endif

}

#endif //RACS_SIMD_POLYBLEP_AVX_F32_HPP
