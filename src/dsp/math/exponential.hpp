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

#ifndef RACS_EXPONENTIAL_HPP
#define RACS_EXPONENTIAL_HPP

#include "../simd/exponential.hpp"
#include "../core/tensor.hpp"
#include "../runtime/exceptions.hpp"


namespace racs::math {

template<int NDIMS, typename T>
tensor<NDIMS, T> exp(const tensor<NDIMS, T> &x);

template<int NDIMS, typename T>
tensor<NDIMS, T> power(const tensor<NDIMS, T> &x, const tensor<NDIMS, T> &y);

template<int NDIMS, typename T1, typename T2>
tensor<NDIMS, T1> power(const tensor<NDIMS, T1> &x, T2 y);

template<int NDIMS, typename T1, typename T2 >
tensor<NDIMS, T2> power(T1 x, const tensor<NDIMS, T2> &y);

}


#endif //RACS_EXPONENTIAL_HPP
