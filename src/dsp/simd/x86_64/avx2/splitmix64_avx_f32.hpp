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

#ifndef RACS_SIMD_SPLITMIX64_AVX_F32_HPP
#define RACS_SIMD_SPLITMIX64_AVX_F32_HPP

#include "../../constants.hpp"
#include <cstdint>

namespace racs::simd {

#ifdef __AVX2__

struct splitmix64_state_f32 {
    __m256i_u _x;
};

alignas(32) const uint64_t c0[4] = { 0x9e3779b97f4a7c15ull, 0x9e3779b97f4a7c15ull,
                                     0x9e3779b97f4a7c15ull, 0x9e3779b97f4a7c15ull };

alignas(32) const uint64_t c1[4] = { 0xbf58476d1ce4e5b9, 0xbf58476d1ce4e5b9,
                                     0xbf58476d1ce4e5b9, 0xbf58476d1ce4e5b9 };

alignas(32) const uint64_t c2[4] = { 0x94d049bb133111eb, 0x94d049bb133111eb,
                                     0x94d049bb133111eb, 0x94d049bb133111eb };

const __m256i_u _c0 = _mm256_loadu_si256(reinterpret_cast<const __m256i_u *>(c0));

const __m256i_u _c1 = _mm256_loadu_si256(reinterpret_cast<const __m256i_u *>(c1));

const __m256i_u _c2 = _mm256_loadu_si256(reinterpret_cast<const __m256i_u *>(c2));

RACS_SIMD __m256i_u RACS_VECTORCALL _mm256_mul_epu64(__m256i_u _x, __m256i_u _y) {
    __m128i_u _x_lo = _mm256_castsi256_si128(_x);
    __m128i_u _x_hi = _mm256_extracti128_si256(_x, 1);
    __m128i_u _y_lo = _mm256_castsi256_si128(_y);
    __m128i_u _y_hi = _mm256_extracti128_si256(_y, 1);

    __m128i_u _r_lo = _mm_mul_epu32(_x_lo, _y_lo);
    __m128i_u _r_hi = _mm_mul_epu32(_x_hi, _y_hi);

    return _mm256_inserti128_si256(_mm256_castsi128_si256(_r_lo), _r_hi, 1);
}

template<int offset>
RACS_SIMD __m256i_u RACS_VECTORCALL _mm256_splitmix64_epu64(struct splitmix64_state_f32* x) {
    __m256i_u _x = x->_x;
    __m256i_u _z = _mm256_castsi128_si256(_mm256_extracti128_si256(_x, offset));

    _z = _mm256_add_epi64(_z, _c0);
    _z = _mm256_xor_si256(_z, _mm256_srli_epi64(_z, 30));
    _z = _mm256_mul_epu64(_z, _c1);
    _z = _mm256_xor_si256(_z, _mm256_srli_epi64(_z, 27));
    _z = _mm256_mul_epu64(_z, _c2);

    return _mm256_xor_si256(_z, _mm256_srli_epi64(_z, 31));
}
#endif

}

#endif //RACS_SIMD_SPLITMIX64_AVX_F32_HPP
