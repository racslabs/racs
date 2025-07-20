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

#ifndef RACS_SIMD_BESSEL_AVX_F32_HPP
#define RACS_SIMD_BESSEL_AVX_F32_HPP

#include "../../bessel.hpp"

namespace racs::simd {

#ifdef __AVX2__

RACS_SIMD __m256 RACS_VECTORCALL _mm256_i0_ps(__m256 _x) {
    const __m256 _c0 = _mm256_set1_ps(1);
    const __m256 _c1 = _mm256_set1_ps(3.5156229);
    const __m256 _c2 = _mm256_set1_ps(3.0899424);
    const __m256 _c3 = _mm256_set1_ps(1.2067492);
    const __m256 _c4 = _mm256_set1_ps(0.2659732);
    const __m256 _c5 = _mm256_set1_ps(0.360768e-1);
    const __m256 _c6 = _mm256_set1_ps(0.45813e-2);

    const __m256 _s0 = _mm256_set1_ps(0.39894228);
    const __m256 _s1 = _mm256_set1_ps(0.1328592e-1);
    const __m256 _s2 = _mm256_set1_ps(0.225319e-2);
    const __m256 _s3 = _mm256_set1_ps(-0.157565e-2);
    const __m256 _s4 = _mm256_set1_ps(0.916281e-2);
    const __m256 _s5 = _mm256_set1_ps(-0.2057706e-1);
    const __m256 _s6 = _mm256_set1_ps(0.2635537e-1);
    const __m256 _s7 = _mm256_set1_ps(-0.1647633e-1);
    const __m256 _s8 = _mm256_set1_ps(0.392377e-2);

    const __m256 _k0 = _mm256_set1_ps(3.75);

    __m256 _ax = _mm256_abs_ps(_x);

    __m256 _x0;

    _x0 = _mm256_div_ps(_x, _k0);
    _x0 = _mm256_mul_ps(_x0, _x0);

    __m256 _p0;

    _p0 = _mm256_fmadd_ps(_c6, _x0, _c5);
    _p0 = _mm256_fmadd_ps(_p0, _x0, _c4);
    _p0 = _mm256_fmadd_ps(_p0, _x0, _c3);
    _p0 = _mm256_fmadd_ps(_p0, _x0, _c2);
    _p0 = _mm256_fmadd_ps(_p0, _x0, _c1);
    _p0 = _mm256_fmadd_ps(_p0, _x0, _c0);

    __m256 _x1 = _mm256_div_ps(_k0, _ax);

    __m256 _e_ax = _mm256_exp_ps(_ax);
    __m256 _s_ax = _mm256_sqrt_ps(_ax);
    __m256 _t0 = _mm256_div_ps(_e_ax, _s_ax);

    __m256 _p1;

    _p1 = _mm256_fmadd_ps(_s8, _x1, _s7);
    _p1 = _mm256_fmadd_ps(_p1, _x1, _s6);
    _p1 = _mm256_fmadd_ps(_p1, _x1, _s5);
    _p1 = _mm256_fmadd_ps(_p1, _x1, _s4);
    _p1 = _mm256_fmadd_ps(_p1, _x1, _s3);
    _p1 = _mm256_fmadd_ps(_p1, _x1, _s2);
    _p1 = _mm256_fmadd_ps(_p1, _x1, _s1);
    _p1 = _mm256_fmadd_ps(_p1, _x1, _s0);
    _p1 = _mm256_mul_ps(_t0, _p1);

    __m256 _mask = _mm256_cmp_ps(_ax, _k0, _CMP_LT_OS);

    return _mm256_blendv_ps(_p1, _p0, _mask);
}

RACS_SIMD __m256 RACS_VECTORCALL _mm256_k0_ps(__m256 _x) {
    const __m256 _c0 = _mm256_set1_ps(-0.57721566);
    const __m256 _c1 = _mm256_set1_ps(0.42278420);
    const __m256 _c2 = _mm256_set1_ps(0.23069756);
    const __m256 _c3 = _mm256_set1_ps(0.3488590e-1);
    const __m256 _c4 = _mm256_set1_ps(0.262698e-2);
    const __m256 _c5 = _mm256_set1_ps(0.10750e-3);
    const __m256 _c6 = _mm256_set1_ps(0.74e-5);

    const __m256 _s0 = _mm256_set1_ps(1.25331414);
    const __m256 _s1 = _mm256_set1_ps(-0.7832358e-1);
    const __m256 _s2 = _mm256_set1_ps(0.2189568e-1);
    const __m256 _s3 = _mm256_set1_ps(-0.1062446e-1);
    const __m256 _s4 = _mm256_set1_ps(0.587872e-2);
    const __m256 _s5 = _mm256_set1_ps(-0.251540e-2);
    const __m256 _s6 = _mm256_set1_ps(0.53208e-3);

    const __m256 _k0 = _mm256_set1_ps(-1);
    const __m256 _k1 = _mm256_set1_ps(2);
    const __m256 _k2 = _mm256_set1_ps(4);


    __m256 _x0;
    _x0 = _mm256_mul_ps(_x, _x);
    _x0 = _mm256_div_ps(_x0, _k2);

    __m256 _x_ln;
    _x_ln = _mm256_div_ps(_x, _k1);
    _x_ln = _mm256_log_ps(_x_ln);
    _x_ln = _mm256_mul_ps(_x_ln, _k0);

    __m256 _x_i0 = _mm256_i0_ps(_x);
    __m256 _t0 = _mm256_mul_ps(_x_ln, _x_i0);

    __m256 _p0;

    _p0 = _mm256_fmadd_ps(_c6, _x0, _c5);
    _p0 = _mm256_fmadd_ps(_p0, _x0, _c4);
    _p0 = _mm256_fmadd_ps(_p0, _x0, _c3);
    _p0 = _mm256_fmadd_ps(_p0, _x0, _c2);
    _p0 = _mm256_fmadd_ps(_p0, _x0, _c1);
    _p0 = _mm256_fmadd_ps(_p0, _x0, _c0);
    _p0 = _mm256_add_ps(_p0, _t0);

    __m256 _x1 = _mm256_div_ps(_k1, _x);

    __m256 _e_x;
    _e_x = _mm256_mul_ps(_x, _k0);
    _e_x = _mm256_exp_ps(_e_x);

    __m256 _x_s = _mm256_sqrt_ps(_x);
    __m256 _t1 = _mm256_div_ps(_e_x, _x_s);

    __m256 _p1;

    _p1 = _mm256_fmadd_ps(_s6, _x1, _s5);
    _p1 = _mm256_fmadd_ps(_p1, _x1, _s4);
    _p1 = _mm256_fmadd_ps(_p1, _x1, _s3);
    _p1 = _mm256_fmadd_ps(_p1, _x1, _s2);
    _p1 = _mm256_fmadd_ps(_p1, _x1, _s1);
    _p1 = _mm256_fmadd_ps(_p1, _x1, _s0);
    _p1 = _mm256_mul_ps(_p1, _t1);

    __m256 _mask = _mm256_cmp_ps(_x, _k1, _CMP_LE_OS);

    return _mm256_blendv_ps(_p1, _p0, _mask);
}
#endif

}

#endif //RACS_SIMD_BESSEL_AVX_F32_HPP
