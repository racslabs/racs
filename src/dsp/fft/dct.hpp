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

#ifndef RACS_DCT_HPP
#define RACS_DCT_HPP

#include "../math/complex.hpp"
#include "fft.hpp"
#include "../simd/dct.hpp"

namespace racs::fft {

template<typename T>
tensor<1, T> dct(const tensor<1, T> &in);

template<typename T>
tensor<1, T> idct(const tensor<1, T> &in);

template<typename T>
tensor<1, T> _dct_base(const tensor<1, T> &in);

template<typename T>
tensor<1, T> _idct_base(const tensor<1, T>& in);

template<typename T>
tensor<1, T> _dct_even_odd(const tensor<1, T> &in);

template<typename T>
tensor<1, T> _idct_even_odd(const tensor<1, T> &in);

}

#endif //RACS_DCT_HPP
