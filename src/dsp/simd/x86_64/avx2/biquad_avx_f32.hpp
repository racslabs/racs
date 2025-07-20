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

#ifndef RACS_SIMD_BIQUAD_AVX_F32_HPP
#define RACS_SIMD_BIQUAD_AVX_F32_HPP

#include "../../biquad.hpp"

namespace racs::simd {

#if defined(__AVX2__) || defined(__SSE4_1__)

struct Biquad_config_f32 {
    __m128 c_[10];
};
#endif

#ifdef __AVX2__

RACS_SIMD __m256 RACS_VECTORCALL _mm256_biquadfir_ps(__m256 _w0,
                                                       __m256 _w1,
                                                       __m256 _w2,
                                                       __m256 _b0,
                                                       __m256 _b1,
                                                       __m256 _b2) {
    return _mm256_fmadd_ps(_b2, _w2, _mm256_fmadd_ps(_b1, _w1, _mm256_mul_ps(_b0, _w0)));
}

RACS_SIMD __m128 RACS_VECTORCALL _mm_biquadiir_ps(__m128 _x,
                                                       __m128 _xk,
                                                       __m128 _xk1,
                                                       __m128 _xk2,
                                                       __m128 _wk1,
                                                       __m128 _wk2,
                                                       __m128* _c) {
    __m128 _r;

    _r = _mm_fmadd_ps(_c[0], _wk1, _x);
    _r = _mm_fmadd_ps(_c[1], _wk2, _r);
    _r = _mm_fmadd_ps(_c[2], _xk, _r);
    _r = _mm_fmadd_ps(_c[3], _wk1, _r);
    _r = _mm_fmadd_ps(_c[4], _xk1, _r);
    _r = _mm_fmadd_ps(_c[5], _xk, _r);
    _r = _mm_fmadd_ps(_c[6], _wk2, _r);
    _r = _mm_fmadd_ps(_c[7], _xk2, _r);
    _r = _mm_fmadd_ps(_c[8], _xk1, _r);

    return _mm_fmadd_ps(_c[9], _wk1, _r);
}

#endif
}

#endif //RACS_SIMD_BIQUAD_AVX_F32_HPP
