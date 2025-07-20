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

#ifndef RACS_SIMD_CONSTANTS_HPP
#define RACS_SIMD_CONSTANTS_HPP

#include <cmath>

#if defined(__AVX__) || defined(__SSE4_1__)
    #include <immintrin.h>
#elif defined( __ARM_NEON__)
    #include <arm_neon.h>
#else
    #warning "SIMD instructions not found. Code will not compile."
#endif

#define RACS_ALIGN 32

#ifdef __MSVC__
    #define RACS_INLINE __forceinline
#elif defined(__GNUG__) || defined(__GNUC__)

    #define RACS_INLINE __inline __attribute__((always_inline))
#else
    #define RACS_INLINE inline
#endif

#define RACS_SIMD RACS_INLINE static

#ifdef __MSVC__
    #define RACS_VECTORCALL __vectorcall
#else
    #define RACS_VECTORCALL
#endif

#if defined(__GNUG__) || defined(__GNUC__)
    #define RACS_DO_PRAGMA(x) _Pragma(#x)
    #define RACS_UNROLL(N) RACS_DO_PRAGMA(GCC unroll N)
#endif

#ifdef __ARM_NEON__

using v128 = float32x4_t;
using v128i = int32x4_t;
using v128u = uint32x4_t;
using v64 = float32x2_t;

#endif

namespace racs::simd {

template<typename T>
constexpr static T pi = static_cast<T>(3.141592653589793238462643383279502884L);

template<typename T>
constexpr static T pi2 = static_cast<T>(pi<T> * 2);

template<typename T>
constexpr static T pi2_rec = static_cast<T>(1 / pi2<T>);

template<typename T>
constexpr static T pi_div2 = static_cast<T>(pi<T> / 2);

template<typename T>
constexpr static T sqrt2 = static_cast<T>(1.4142135623730951454746218587388284504413604736328125);

}


#endif //RACS_SIMD_CONSTANTS_HPP
