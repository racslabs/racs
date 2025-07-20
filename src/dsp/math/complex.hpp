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

#ifndef RACS_COMPLEX_HPP
#define RACS_COMPLEX_HPP

#include "../simd/complex.hpp"
#include "../core/generic.hpp"
#include "../core/tensor.hpp"

namespace racs::math {

template<typename T>
tensor<1, T> real(const tensor<1, std::complex<T>>& in);

template<typename T>
tensor<1, T> imag(const tensor<1, std::complex<T>>& in);

template<int NDIMS, typename T>
tensor<NDIMS, std::complex<T>> complex(const tensor<NDIMS, T> &in);

template<int NDIMS, typename T>
tensor<NDIMS, std::complex<T>> cexp(const tensor<NDIMS, std::complex<T>> &x);

template<int NDIMS, typename T>
tensor<NDIMS, std::complex<T>> csinh(const tensor<NDIMS, std::complex<T>>& x);

template<typename T>
tensor<1, std::complex<T>> ccumprod(const tensor<1, std::complex<T>>& x);

template<typename T>
std::complex<T> cprod(const tensor<1, std::complex<T>>& x);

template<int NDIMS, typename T>
tensor<NDIMS, T> cabs(const tensor<NDIMS, std::complex<T>>& in);

template<int NDIMS, typename T>
tensor<NDIMS, std::complex<T>> cconj(const tensor<NDIMS, std::complex<T>>& x);

template<int NDIMS, typename T>
tensor<NDIMS, std::complex<T>> csquare(const tensor<NDIMS, std::complex<T>>& x);

template<int NDIMS, typename T>
tensor<NDIMS, std::complex<T>> csqrt(const tensor<NDIMS, std::complex<T>>& x);

}

#endif //RACS_COMPLEX_HPP
