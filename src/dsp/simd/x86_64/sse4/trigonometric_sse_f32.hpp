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

#ifndef RACS_SIMD_TRIGONOMETRIC_SSE_F32_HPP
#define RACS_SIMD_TRIGONOMETRIC_SSE_F32_HPP

#include "../../trigonometric.hpp"

namespace racs::simd {

#if defined(__SSE4_1__)

RACS_SIMD __m128 RACS_VECTORCALL _mm_sin_ps(__m128 _x) {
      const __m128 _a0 = _mm_set1_ps(1);
    const __m128 _a1 = _mm_set1_ps(-0.16666666666);
    const __m128 _a2 = _mm_set1_ps(0.00833333333);
    const __m128 _a3 = _mm_set1_ps(-0.00019841269);
    const __m128 _a4 = _mm_set1_ps(0.00000275573);
    const __m128 _a5 = _mm_set1_ps(-2.50521084e-8);
    const __m128 _a6 = _mm_set1_ps(1.6059044e-10);
    const __m128 _a7 = _mm_set1_ps(-7.6471637e-13);

    const __m128 _c0 = _mm_set1_ps(0.15915494309);
    const __m128 _c1 = _mm_set1_ps(6.28318530718);
    const __m128 _c2 = _mm_set1_ps(3.14159265359);

    __m128 _xf = _mm_floor_ps(_mm_mul_ps(_x, _c0));
    _x = _mm_sub_ps(_x, _mm_mul_ps(_xf, _c1));
    _x = _mm_sub_ps(_c2, _x);

    __m128 _x2 = _mm_mul_ps(_x, _x);
    __m128 _y = _mm_fmadd_ps(_x2, _a7, _a6);
    _y = _mm_fmadd_ps(_x2, _y, _a5);
    _y = _mm_fmadd_ps(_x2, _y, _a4);
    _y = _mm_fmadd_ps(_x2, _y, _a3);
    _y = _mm_fmadd_ps(_x2, _y, _a2);
    _y = _mm_fmadd_ps(_x2, _y, _a1);
    _y = _mm_fmadd_ps(_x2, _y, _a0);

    return _mm_mul_ps(_x, _y);
}

RACS_SIMD __m128 RACS_VECTORCALL _mm_sinc_ps(__m128 _x) {
    const __m128 _pi = _mm_set1_ps(3.14159265359);

    _x = _mm_mul_ps(_x, _pi);
    __m128 _n = _mm_sin_ps(_x);

    return _mm_div_ps(_n, _x);
}

RACS_SIMD __m128 RACS_VECTORCALL _mm_cos_ps(__m128 _x) {
    const __m128 _c0 = _mm_set1_ps(1.57079632679);
    return _mm_sin_ps(_mm_sub_ps(_c0, _x));
}

RACS_SIMD __m128 RACS_VECTORCALL _mm_tan_ps(__m128 _a) {
    __m128 _b = _mm_sin_ps(_a);
    __m128 _c = _mm_cos_ps(_a);
    return _mm_div_ps(_b, _c);
}

RACS_SIMD __m128 RACS_VECTORCALL _mm_cot_ps(__m128 _a) {
    __m128 _b = _mm_cos_ps(_a);
    __m128 _c = _mm_sin_ps(_a);
    return _mm_div_ps(_b, _c);
}

RACS_SIMD __m128 RACS_VECTORCALL _mm_sec_ps(__m128 _a) {
    const __m128 _v = _mm_set1_ps(1.0);
    return _mm_div_ps(_v, _mm_cos_ps(_a));
}

RACS_SIMD __m128 RACS_VECTORCALL _mm_csc_ps(__m128 _a) {
    const __m128 _v = _mm_set1_ps(1.0);
    return _mm_div_ps(_v, _mm_sin_ps(_a));
}
#endif

}


#endif //RACS_SIMD_TRIGONOMETRIC_SSE_F32_HPP
