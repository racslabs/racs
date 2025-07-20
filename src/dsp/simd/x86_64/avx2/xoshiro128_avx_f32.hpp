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

#ifndef RACS_SIMD_XOSHIRO128_AVX_F32_HPP
#define RACS_SIMD_XOSHIRO128_AVX_F32_HPP

#include <cstdint>
#include <random>
#include <iostream>

#include "../../constants.hpp"
#include "../../splitmix64.hpp"

namespace racs::simd {

#ifdef __AVX2__

struct xoshiro128_state {
    __m256i_u _s[4];
};

struct xoshiro128_seed {
    __m256i_u _s[2];
};

RACS_SIMD __m256i_u RACS_VECTORCALL _mm256_rotl32_epu32(const __m256i_u _x, int k) {
    auto _a = _mm256_slli_epi32(_x, k);
    auto _b = _mm256_srli_epi32(_x, 32 - k);
    return _mm256_or_si256(_a, _b);
}

RACS_SIMD __m256i_u RACS_VECTORCALL _mm256_xoshiro128_epu32(struct xoshiro128_state* s) {
    __m256i_u* _s = s->_s;
    const __m256i_u _r = _mm256_rotl32_epu32(_mm256_add_epi32(_s[0], _s[3]), 7);
    const __m256i_u _t = _mm256_slli_epi32(_s[1], 9);

    _s[2] = _mm256_xor_si256(_s[2], _s[0]);
    _s[3] = _mm256_xor_si256(_s[3], _s[1]);
    _s[1] = _mm256_xor_si256(_s[1], _s[2]);
    _s[0] = _mm256_xor_si256(_s[0], _s[3]);

    _s[2] = _mm256_xor_si256(_s[2], _t);
    _s[3] = _mm256_rotl32_epu32(_s[3], 11);

    return _r;
}

RACS_SIMD void xoshiro128_init_state(struct xoshiro128_state* x, struct xoshiro128_seed* s) {
    splitmix64_state_f32 lo{}, hi{};
    lo._x = s->_s[0];
    hi._x = s->_s[1];

    __m256i_u* _x = x->_s;

    __m256i_u _tmp_lo = _mm256_splitmix64_epu64<0>(&lo);
    __m256i_u _tmp_hi = _mm256_splitmix64_epu64<1>(&hi);

    __m128i_u _x0_lo = _mm256_castsi256_si128(_tmp_lo);
    __m128i_u _x0_hi = _mm256_castsi256_si128(_tmp_hi);
    _x[0] = _mm256_inserti128_si256(_x[0], _x0_lo, 0);
    _x[0] = _mm256_inserti128_si256(_x[0], _x0_hi, 1);

    __m128i_u _x1_lo = _mm256_castsi256_si128(_mm256_srli_si256(_tmp_lo, 32));
    __m128i_u _x1_hi = _mm256_castsi256_si128(_mm256_srli_si256(_tmp_hi, 32));
    _x[1] = _mm256_inserti128_si256(_x[1], _x1_lo, 0);
    _x[1] = _mm256_inserti128_si256(_x[1], _x1_hi, 1);

    _tmp_lo = _mm256_splitmix64_epu64<0>(&lo);
    _tmp_hi = _mm256_splitmix64_epu64<1>(&hi);

    __m128i_u _x3_lo = _mm256_castsi256_si128(_tmp_lo);
    __m128i_u _x3_hi = _mm256_castsi256_si128(_tmp_hi);
    _x[3] = _mm256_inserti128_si256(_x[3], _x3_lo, 0);
    _x[3] = _mm256_inserti128_si256(_x[3], _x3_hi, 1);

    __m128i_u _x4_lo = _mm256_castsi256_si128(_mm256_srli_si256(_tmp_lo, 32));
    __m128i_u _x4_hi = _mm256_castsi256_si128(_mm256_srli_si256(_tmp_hi, 32));
    _x[4] = _mm256_inserti128_si256(_x[4], _x4_lo, 0);
    _x[4] = _mm256_inserti128_si256(_x[4], _x4_hi, 1);
}
#endif

}

#endif //RACS_SIMD_XOSHIRO128_AVX_F32_HPP
