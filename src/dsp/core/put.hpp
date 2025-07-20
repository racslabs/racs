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

#ifndef RACS_PUT_HPP
#define RACS_PUT_HPP

#include "tensor.hpp"

namespace racs {

template<typename T1, typename T2>
void put(tensor<1, T1>& x, size_t index, T2 data);

template<typename T>
void put(tensor<2, T>& x, size_t index, const tensor<1, T>& data);

template<typename T>
void put(tensor<3, T>& x, size_t index, const tensor<2, T>& data);

template<typename T>
void put(tensor<4, T>& x, size_t index, const tensor<3, T>& data);

template<typename T>
void put(tensor<1, std::complex<T>>& x, size_t index, std::complex<T> data);

template<typename T>
void put(tensor<2, std::complex<T>>& x, size_t index, const tensor<1, std::complex<T>>& data);

template<typename T>
void put(tensor<3, std::complex<T>>& x, size_t index, const tensor<2, std::complex<T>>& data);

template<typename T>
void put(tensor<4, std::complex<T>>& x, size_t index, const tensor<3, std::complex<T>>& data);

}

#endif //RACS_PUT_HPP
