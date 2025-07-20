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

#ifndef RACS_WAVEDEC_HPP
#define RACS_WAVEDEC_HPP

#include "dwt.hpp"

namespace racs::dwt {

size_t _wavedec_max_depth(size_t n, size_t m);

template<typename T>
std::vector<tensor<1, T>> wavedec(const tensor<1, T>& in, size_t n, WAVELET name);

template<typename T>
tensor<1, T> waverec(const std::vector<tensor<1, T>>& dec, WAVELET name);

template<typename T>
tensor<1, T> upcoef(const std::vector<tensor<1, T>>& dec, size_t n, WAVELET name);

template<typename T>
tensor<1, T> detcoef(const std::vector<tensor<1, T>>& dec, size_t n);

}

#endif //RACS_WAVEDEC_HPP
