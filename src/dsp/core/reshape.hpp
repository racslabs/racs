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

#ifndef RACS_RESHAPE_HPP
#define RACS_RESHAPE_HPP

#include "tensor.hpp"
#include "../runtime/exceptions.hpp"

#define RACS_CHECK_SHAPE_COMPATIBLE(x, y) \
    auto _x = std::accumulate(x.begin(), x.end(), 1, std::multiplies<size_t>()); \
    auto _y = std::accumulate(y.begin(), y.end(), 1, std::multiplies<size_t>()); \
    std::stringstream ss;                  \
    if (_x != _y) {                        \
        ss << x << " is not compatible with " << y;                              \
        throw runtime::zf101_exception(ss.str());                                \
    }

namespace racs {

template<int NDIMS0, int NDIMS1, typename T>
tensor<NDIMS1, T> reshape(const tensor<NDIMS0, T>& x, const tensor_shape& shape);

template<int NDIMS0, int NDIMS1, typename T>
tensor<NDIMS1, std::complex<T>> reshape(const tensor<NDIMS0, std::complex<T>>& x, const tensor_shape& shape);

}

#endif //RACS_RESHAPE_HPP
