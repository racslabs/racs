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

#ifndef RACS_DOT_PRODUCT_HPP
#define RACS_DOT_PRODUCT_HPP

#include "../simd/linalg.hpp"
#include "../core/tensor.hpp"
#include "../core/op.hpp"
#include "../runtime/exceptions.hpp"
#include "transpose.hpp"

namespace racs::linalg {

#define RACS_CHECK_COMPATIBLE_SHAPE(x, y) \
    std::stringstream ss;                  \
    auto n = x.shape().ndims();            \
    auto m = y.shape().ndims();            \
    if (x.shape()[n - 1] != y.shape()[m - 2]) { \
        ss << x.shape() << " is incompatible with " << y.shape(); \
        throw runtime::zf101_exception(ss.str());\
    }

template<typename T>
T dot(const tensor<1, T>& x, const tensor<1, T>& y);

template<typename T>
tensor<1, T> dot(const tensor<1, T>& x, const tensor<2, T>& y);

template<typename T>
tensor<2, T> dot(const tensor<2, T>& x, const tensor<2, T>& y);


}

#endif //RACS_DOT_PRODUCT_HPP
