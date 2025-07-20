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

#ifndef RACS_SIMD_TRIGONOMETRIC_AVX_F32_HPP
#define RACS_SIMD_TRIGONOMETRIC_AVX_F32_HPP

#include "../../trigonometric.hpp"
#include "../../abs_floor.hpp"

namespace racs::simd {

#ifdef __AVX2__

RACS_SIMD __m256 RACS_VECTORCALL _mm256_sin_ps(__m256 _x) {
    const __m256 _a0 = _mm256_set1_ps(1);
    const __m256 _a1 = _mm256_set1_ps(-0.166666666666666666666666666666666666666666666666666666666666666);
    const __m256 _a2 = _mm256_set1_ps(0.0083333333333333333333333333333333333333333333333333333333333333);
    const __m256 _a3 = _mm256_set1_ps(-0.000198412698412698412698412698412698412698412698412698412698412);
    const __m256 _a4 = _mm256_set1_ps(2.7557319223985890652557319223985890652557319223985890652557e-6);
    const __m256 _a5 = _mm256_set1_ps(-2.505210838544171877505210838544171877505210838544171877505e-8);
    const __m256 _a6 = _mm256_set1_ps(1.6059043836821614599392377170154947932725710503488281266059e-10);
    const __m256 _a7 = _mm256_set1_ps(-7.64716373181981647590113198578807044415510023975632441240e-13);
    const __m256 _a8 = _mm256_set1_ps(2.8114572543455207631989455830103200162334927352045310339739e-15);

    const __m256 _c0 = _mm256_set1_ps(0.1591549430918953357688837633725143620344596457404564487476673440);
    const __m256 _c1 = _mm256_set1_ps(6.2831853071795864769252867665590057683943387987502116419498891846);
    const __m256 _c2 = _mm256_set1_ps(3.1415926535897932384626433832795028841971693993751058209749445923);

    __m256 _xf = _mm256_floor_ps(_mm256_mul_ps(_x, _c0));
    _x = _mm256_sub_ps(_x, _mm256_mul_ps(_xf, _c1));
    _x = _mm256_sub_ps(_c2, _x);

    __m256 _x2 = _mm256_mul_ps(_x, _x);
    __m256 _y = _mm256_fmadd_ps(_x2, _a8, _a7);
    _y = _mm256_fmadd_ps(_x2, _y, _a6);
    _y = _mm256_fmadd_ps(_x2, _y, _a5);
    _y = _mm256_fmadd_ps(_x2, _y, _a4);
    _y = _mm256_fmadd_ps(_x2, _y, _a3);
    _y = _mm256_fmadd_ps(_x2, _y, _a2);
    _y = _mm256_fmadd_ps(_x2, _y, _a1);
    _y = _mm256_fmadd_ps(_x2, _y, _a0);

    return _mm256_mul_ps(_x, _y);

}

RACS_SIMD __m256 RACS_VECTORCALL _mm256_sinc_ps(__m256 _x) {
    const __m256 _pi = _mm256_set1_ps(3.1415926535897932384626433832795028841971693993751058209749445923);
    const __m256 _c0 = _mm256_setzero_ps();
    const __m256 _c1 = _mm256_set1_ps(1);

    __m256 _mask = _mm256_cmp_ps(_x , _c0, _CMP_EQ_OQ);

    _x = _mm256_mul_ps(_x, _pi);

    __m256 _n = _mm256_sin_ps(_x);
    _n = _mm256_div_ps(_n, _x);

    return _mm256_blendv_ps(_n, _c1, _mask);
}

RACS_SIMD __m256 RACS_VECTORCALL _mm256_cos_ps(__m256 _x) {
    const __m256 _c0 = _mm256_set1_ps(1.5707963267948966192313216916397514420985846996875529104874722961);
    return _mm256_sin_ps(_mm256_sub_ps(_c0, _x));
}

RACS_SIMD __m256 RACS_VECTORCALL _mm256_tan_ps(__m256 _a) {
    __m256 _b = _mm256_sin_ps(_a);
    __m256 _c = _mm256_cos_ps(_a);
    return _mm256_div_ps(_b, _c);
}

RACS_SIMD __m256 RACS_VECTORCALL _mm256_cot_ps(__m256 _a) {
    __m256 _b = _mm256_cos_ps(_a);
    __m256 _c = _mm256_sin_ps(_a);
    return _mm256_div_ps(_b, _c);
}

RACS_SIMD __m256 RACS_VECTORCALL _mm256_sec_ps(__m256 _a) {
    const __m256 _v = _mm256_set1_ps(1.0);
    return _mm256_div_ps(_v, _mm256_cos_ps(_a));
}

RACS_SIMD __m256 RACS_VECTORCALL _mm256_csc_ps(__m256 _a) {
    const __m256 _v = _mm256_set1_ps(1.0);
    return _mm256_div_ps(_v, _mm256_sin_ps(_a));
}
#endif

}

#endif //RACS_SIMD_TRIGONOMETRIC_AVX_F32_HPP
