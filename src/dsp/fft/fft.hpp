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

#ifndef RACS_FFT_HPP
#define RACS_FFT_HPP

#include "../simd/fft.hpp"
#include "../core/generic.hpp"
#include "../core/tensor.hpp"
#include "../core/slice.hpp"
#include "../core/op.hpp"
#include "../core/put.hpp"
#include "../core/reshape.hpp"
#include "../linalg/transpose.hpp"
#include "../runtime/session.hpp"

namespace racs::fft {

template<typename T>
tensor<1, std::complex<T>> fft(const tensor<1, std::complex<T>>& in);

template<typename T>
tensor<1, std::complex<T>> ifft(const tensor<1, std::complex<T>>& in);

template<typename T>
tensor<2, std::complex<T>> fft(const tensor<2, std::complex<T>>& in);

template<typename T>
tensor<2, std::complex<T>> ifft(const tensor<2, std::complex<T>>& in);

template<typename T>
tensor<1, std::complex<T>> rfft(const tensor<1, T>& in);

template<typename T>
tensor<1, T> irfft(const tensor<1, std::complex<T>>& in);

template<typename T>
tensor<2, std::complex<T>> fft2(const tensor<2, std::complex<T>>& in);

template<typename T>
tensor<2, std::complex<T>> _fft_parallel(const tensor<2, std::complex<T>> &in);

template<typename T>
tensor<2, std::complex<T>> _fft_parallel_multithreaded(const tensor<2, std::complex<T>> &in);

template<typename T>
tensor<2, std::complex<T>> _fft2_base(const tensor<2, std::complex<T>>& in);

template<typename T>
tensor<2, std::complex<T>> ifft2(const tensor<2, std::complex<T>>& in);

template<typename T>
tensor<2, std::complex<T>> _ifft_parallel(const tensor<2, std::complex<T>> &in);

template<typename T>
tensor<2, std::complex<T>> _ifft_parallel_multithreaded(const tensor<2, std::complex<T>> &in);

template<typename T>
tensor<2, std::complex<T>> _ifft2_base(const tensor<2, std::complex<T>>& in);

template<typename T>
tensor<1, std::complex<T>> _radix2(const tensor<1, std::complex<T>>& in,
                                   const tensor<1, std::complex<T>>& wm,
                                   int n);

template<typename T>
tensor<1, std::complex<T>> _twiddle(size_t n, simd::FFT_TYPE type);

template<typename T>
tensor<1, std::complex<T>> _fft_base(const tensor<1, std::complex<T>>& in);

template<typename T>
tensor<1, std::complex<T>> _ifft_base(const tensor<1, std::complex<T>>& in);

template<typename T>
tensor<1, std::complex<T>> fftshift(const tensor<1, std::complex<T>>& in);

template<typename T>
tensor<1, T> fftfreq(size_t bins, T fs);

}

#endif //RACS_FFT_HPP
