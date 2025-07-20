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

#ifndef RACS_POLYNOMIAL_HPP
#define RACS_POLYNOMIAL_HPP

#include "../core/tensor.hpp"
#include "../core/op.hpp"
#include "../simd/polynomial.hpp"
#include "../fft/convolution.hpp"
#include "constants.hpp"
#include "trigonometric.hpp"
#include "exponential.hpp"

namespace racs::math {

template<typename T>
tensor<1, T> polyval(const tensor<1, T>& p, const tensor<1, T>& x);

template<typename T>
tensor<1, std::complex<T>> polyval(const tensor<1, std::complex<T>>& p,
                                   const tensor<1, std::complex<T>>& x);

template<typename T>
tensor<1, std::complex<T>> poly(const tensor<1, std::complex<T>>& z);

template<typename T>
tensor<1, std::complex<T>> roots(const tensor<1, T>& p);

template<typename T>
tensor<1, std::complex<T>> roots(const tensor<1, std::complex<T>>& p);

template<typename T>
tensor<1, std::complex<T>> unityroots(size_t n, T r, T c);

}


#endif //RACS_POLYNOMIAL_HPP
