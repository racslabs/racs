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

#ifndef RACS_SIMD_GENERIC_HPP
#define RACS_SIMD_GENERIC_HPP

#include <limits>
#include <iostream>
#include <complex>
#include <cmath>
#include "constants.hpp"
#include "abs_floor.hpp"
#include "x86_64/avx2/generic_avx_f32.hpp"
#include "x86_64/sse4/generic_sse_f32.hpp"
#include "arm64/neon/generic_neon_f32.hpp"

namespace racs::simd {

void add(float *a, float *b, float *c, size_t n);

void sub(float *a, float *b, float *c, size_t n);

void mul(float *a, float *b, float *c, size_t n);

void div(float *a, float *b, float *c, size_t n);

void mod(float *a, float *b, float *c, size_t n);

void rev(float* a, float* b, size_t n);

float mod(float a, float b);

void crev(std::complex<float>* a, std::complex<float>* b, size_t n);

void cmplxmul(std::complex<float>* a, std::complex<float>* b, std::complex<float>* c, size_t n);

void cmplxdiv(std::complex<float>* a, std::complex<float>* b, std::complex<float>* c, size_t n);

void dilate(float* a, float* b, size_t n, size_t d);

void interleaved(const float *q0, const float *q1, float *q2, size_t n);

void planar(const float *q0, float *q1, size_t n);

}

#endif //RACS_SIMD_GENERIC_HPP
