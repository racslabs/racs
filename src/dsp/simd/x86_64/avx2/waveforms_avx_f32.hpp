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

#ifndef RACS_SIMD_WAVEFORMS_AVX_F32_HPP
#define RACS_SIMD_WAVEFORMS_AVX_F32_HPP

#include "waveforms_avx_f32.hpp"

namespace racs::simd {

#ifdef __AVX2__

RACS_SIMD __m256 RACS_VECTORCALL _mm256_blsaw_ps(__m256 _p, __m256 _pd) {
    const __m256 _v0 = _mm256_set1_ps(pi2_rec<float>);
    const __m256 _v1 = _mm256_set1_ps(2.0f);
    const __m256 _v2 = _mm256_set1_ps(1.0f);

    __m256 _t, _a;

    _t = _mm256_mul_ps(_p, _v0);
    _a = _mm256_sub_ps(_mm256_mul_ps(_t, _v1), _v2);

    return _mm256_sub_ps(_a, _mm256_polyblep_ps(_pd, _t));
}

RACS_SIMD __m256 RACS_VECTORCALL _mm256_square_ps(__m256 _p) {
    const __m256 _v0 = _mm256_set1_ps(pi<float>);
    const __m256 _v1 = _mm256_set1_ps(1.0f);
    const __m256 _v2 = _mm256_set1_ps(-1.0f);

    __m256 _mask = _mm256_cmp_ps(_p, _v0, _CMP_LT_OQ);
    return _mm256_blendv_ps(_v2, _v1, _mask);
}

RACS_SIMD __m256 RACS_VECTORCALL _mm256_blsquare_ps(__m256 _p, __m256 _pd) {
    const __m256 _v0 = _mm256_set1_ps(pi2_rec<float>);
    const __m256 _v1 = _mm256_set1_ps(1.0f);
    const __m256 _v2 = _mm256_set1_ps(0.5f);

    __m256 _t = _mm256_mul_ps(_p, _v0);
    __m256 _t0 = _mm256_mod_ps(_mm256_add_ps(_t, _v2), _v1);

    __m256 _a = _mm256_square_ps(_p);
    _a = _mm256_add_ps(_mm256_polyblep_ps(_pd, _t), _a);

    return _mm256_sub_ps(_a, _mm256_polyblep_ps(_pd, _t0));
}
#endif

}

#endif //RACS_SIMD_WAVEFORMS_AVX_F32_HPP
