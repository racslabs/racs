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

#include "../../xoshiro128.hpp"

namespace racs::simd {

#ifdef  __AVX2__

void xoshiro128_init_seed(struct xoshiro128_seed* seed) {
    std::random_device rd;

    alignas(32) uint64_t s0[4] = { rd(), rd(), rd(), rd() };
    alignas(32) uint64_t s1[4] = { rd(), rd(), rd(), rd() };

    seed->_s[0] = _mm256_loadu_si256(reinterpret_cast<__m256i_u *>(s0));
    seed->_s[1] = _mm256_loadu_si256(reinterpret_cast<__m256i_u *>(s1));
}

void xoshiro128(uint32_t* x, size_t n) {
    xoshiro128_seed seed{};
    xoshiro128_init_seed(&seed);

    xoshiro128_state state{};
    xoshiro128_init_state(&state, &seed);

    for (int i = 0; i < n; i += 8) {
        __m256i_u _x = _mm256_xoshiro128_epu32(&state);
        _mm256_storeu_si256(reinterpret_cast<__m256i_u *>(x + i), _x);
    }
}

void xoshiro128(float* x, size_t n) {
    xoshiro128_seed seed{};
    xoshiro128_init_seed(&seed);

    xoshiro128_state state{};
    xoshiro128_init_state(&state, &seed);

    auto max = std::numeric_limits<uint32_t>::max();

    size_t i = 0, j = 0;

    if (n >= 8) {
        for (i = 0; i < n; i += 8) {
            __m256i_u _x = _mm256_xoshiro128_epu32(&state);
            __m256 _y = _mm256_cvtepi32_ps(_x);
            __m256 _z = _mm256_div_ps(_y, _mm256_set1_ps((static_cast<float>(max))));
            _mm256_storeu_ps(x + i, _z);
        }
    }

    // use seed for remainder
    alignas(32) float y[8];
    __m256i_u _x = seed._s[0];

    __m256 _y = _mm256_cvtepi32_ps(_x);
    _mm256_storeu_ps(y, _y);

    for ( ; i < n; ++i, ++j) {
        x[i] = y[j];
    }
}
#endif

}