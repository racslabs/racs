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

#ifndef RACS_SIMD_LINALG_HPP
#define RACS_SIMD_LINALG_HPP

#include <complex>
#include "constants.hpp"
#include "x86_64/avx2/linalg_avx_f32.hpp"
#include "arm64/neon/linalg_neon_f32.hpp"

namespace racs::simd {

float dot(float* a, float* b, size_t n);

float norm(float* x, size_t n);

void transpose(float *a, float *b, int i_s, int j_s, int i_e, int j_e, int n, int m);

void transpose(std::complex<float> *a, std::complex<float> *b, int i_s, int j_s, int i_e, int j_e, int n, int m);

}

#endif //RACS_SIMD_LINALG_HPP
