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

#ifndef RACS_GENERIC_HPP
#define RACS_GENERIC_HPP

#include "../simd/generic.hpp"
#include "../math/cumsum.hpp"

namespace racs {

/**
 * @tparam T The type of the tensor elements.
 * @param x The 1-D input tensor.
 * @return A new 1-D tensor with the elements in reverse order.
 *
 * Example: <br/>
 * @code
 * racs::tensor<1, float> x = {1., 2., 3.};
 *
 * // returns {3., 2., 1.}
 * auto y = racs::reverse(x);
 * @endcode
 */
template<typename T>
tensor<1, T> reverse(const tensor<1, T>& x);

/**
 * @tparam T The type of the real and imaginary parts of the tensor elements.
 * @param x The 1-D input tensor.
 * @return A new complex 1-D tensor with the elements in reverse order.
 *
 * Example: <br/>
 * @code
 * racs::tensor<1, float> x = {{1., 0.}, {2., 0.}, {3., 0.}};
 *
 * // returns {{3., 0.}, {2., 0.}, {1., 0.}}
 * auto y = racs::reverse(x);
 * @endcode
 */
template<typename T>
tensor<1, std::complex<T>> reverse(const tensor<1, std::complex<T>>& x);

template<typename T>
tensor<1, T> concatenate(const tensor<1, T>& a, const tensor<1, T>& b);

template<typename T>
tensor<1, std::complex<T>> concatenate(const tensor<1, std::complex<T>>& a,
                                       const tensor<1, std::complex<T>>& b);

template<typename T>
tensor<1, T> zeros(size_t dim0);

template<typename T>
tensor<2, T> zeros(size_t dim0, size_t dim1);

template<typename T>
tensor<3, T> zeros(size_t dim0, size_t dim1, size_t dim2);

template<typename T>
tensor<1, T> ones(size_t dim0);

template<typename T>
tensor<2, T> ones(size_t dim0, size_t dim1);

template<typename T>
tensor<3, T> ones(size_t dim0, size_t dim1, size_t dim2);

template<typename T>
tensor<1, T> pad(const tensor<1, T>& x, size_t left = 0, size_t right = 0, T c = T(0));

template<typename T>
tensor<2, T> pad(const tensor<2, T>& x, size_t left = 0, size_t right = 0, T c = T(0));

template<typename T>
tensor<1, std::complex<T>> pad(const tensor<1, std::complex<T>>& x, size_t left = 0, size_t right = 0, std::complex<T> c = 0);

template<typename T>
tensor<2, std::complex<T>> pad(const tensor<2, std::complex<T>>& x, size_t left = 0, size_t right = 0, std::complex<T> c = 0);

template<typename T>
tensor<1, T> dilate(const tensor<1, T>& x, size_t d = 1);

template<typename T>
tensor<2, T> dilate(const tensor<2, T>& x, size_t d = 1);

template<typename T>
tensor<3, T> dilate(const tensor<3, T>& x, size_t d = 1);

template<typename T>
tensor<1, T> linspace(T start, T stop, size_t num);

template<typename T>
tensor<1, T> arange(T start, T stop, T step = 1);

template<typename T>
tensor<1, T> interleaved(const tensor<2, T>& x);

template<typename T>
tensor<2, T> planar(const tensor<1, T>& x);

}

#endif //RACS_GENERIC_HPP
