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

#ifndef RACS_SIMD_COMPLEX_HPP
#define RACS_SIMD_COMPLEX_HPP

#include "trigonometric.hpp"
#include "exponential.hpp"
#include "generic.hpp"
#include "x86_64/avx2/complex_avx_f32.hpp"
#include "x86_64/sse4/complex_sse_f32.hpp"
#include "arm64/neon/complex_neon_f32.hpp"

#include <complex>

namespace racs::simd {

void complex(float* x, std::complex<float>* y, size_t n);

void cabs(std::complex<float>* x, float* y, size_t n);

void real(std::complex<float>* x, float* y, size_t n);

void imag(std::complex<float>* x, float* y, size_t n);

void ccumprod(const std::complex<float> *a, std::complex<float> *b, size_t n);

void cexp(std::complex<float>* a, std::complex<float>* b, size_t n);

void csinh(std::complex<float>* a, std::complex<float>* b, size_t n);

void cconj(std::complex<float>* a, std::complex<float>* b, size_t n);

void csquare(std::complex<float>* a, std::complex<float>* b, size_t n);

void csqrt(std::complex<float>* a, std::complex<float>* b, size_t n);

}

#endif //RACS_SIMD_COMPLEX_HPP
