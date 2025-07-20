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

#ifndef RACS_SIMD_ELU_HPP
#define RACS_SIMD_ELU_HPP

#include "exponential.hpp"
#include "x86_64/avx2/elu_avx2_f32.hpp"
#include "arm64/neon/elu_neon_f32.hpp"

namespace racs::simd {

void elu(float* x, float* y, float a, size_t n);

}

#endif //RACS_SIMD_ELU_HPP
