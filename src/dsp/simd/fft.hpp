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

#ifndef RACS_SIMD_FFT_HPP
#define RACS_SIMD_FFT_HPP

#include <cmath>
#include <iostream>
#include <vector>
#include "generic.hpp"
#include "complex.hpp"
#include "x86_64/avx2/fft_avx_f32.hpp"
#include "x86_64/sse4/fft_sse_f32.hpp"
#include "arm64/neon/fft_neon_f32.hpp"

namespace racs::simd {

enum class FFT_TYPE : unsigned {
    INVERSE = 0,
    FORWARD = 1
};

void twiddle4x32(std::complex<float>* x, FFT_TYPE type, size_t n);

void twiddle2x32(std::complex<float>* x, FFT_TYPE type, size_t n);

void twiddle(std::complex<float>* x, FFT_TYPE type, size_t n);

void bit_reverse_permute8x32(std::complex<float>* x, std::complex<float>* y, int n, int m);

void bit_reverse_permute4x32(std::complex<float>* x, std::complex<float>* y, int n, int m);

void bit_reverse_permute(std::complex<float>* x, std::complex<float>* y, int n, int m);

void butterfly4x32(std::complex<float>* x, std::complex<float> w, size_t n, size_t m);

void butterfly2x32(std::complex<float>* x, std::complex<float> w, size_t n, size_t m);

void butterfly1x32(std::complex<float>* x, size_t n);

void butterfly(std::complex<float>* x, std::complex<float> w, size_t n, size_t m);

void radix2(std::complex<float>* x, std::complex<float>* y, std::complex<float>* wm, int n);

}


#endif //RACS_SIMD_FFT_HPP
