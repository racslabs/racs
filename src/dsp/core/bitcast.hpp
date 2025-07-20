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

#ifndef RACS_BITCAST_HPP
#define RACS_BITCAST_HPP

#include "tensor.hpp"

namespace racs {

/**
  * @defgroup bitcast
  * @{
  */

/**
 * @brief Bitcasts a tensor from type `float` to the target type without copying the data.
 * @tparam T The target type to which the tensor should be bitcasted.
 * @param in Input tensor of type `float`
 * @note Bitcast internally performs a reinterpret cast. This operation will yield different results depending on the
 * endianness of the machine.
 * @return A tensor of type `T`.
 */
template<typename T>
tensor<1, T> bitcast(const tensor<1, float>& in);

/**
 * @brief Bitcasts a tensor from type `std::complex<float>` to the target type without copying the data.
 * @tparam T The target type to which the tensor should be bitcasted.
 * @param in Input tensor of type `std::complex<float>`
 * @note Bitcast internally performs a reinterpret cast. This operation will yield different results depending on the
 * endianness of the machine.
 * @return A tensor of type `T`.
 */
template<typename T>
tensor<1, T> bitcast(const tensor<1, std::complex<float>>& in);

/** @} */

}


#endif //RACS_BITCAST_HPP
