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

#include "dot.hpp"

namespace racs::linalg {

template<typename T>
T dot(const tensor<1, T>& x, const tensor<1, T>& y) {
    RACS_CHECK_EQUAL_SHAPE(x, y)

    return simd::dot(x.data(), y.data(), x.size());
}

template<typename T>
tensor<1, T> dot(const tensor<1, T>& x, const tensor<2, T>& y) {
    RACS_CHECK_COMPATIBLE_SHAPE(x, y)

    auto yt = transpose(y);
    auto dim0 = yt.shape()[0];
    auto dim1 = yt.shape()[1];

    tensor<1, T> out(tensor_shape({ dim0 }));

    for (size_t i = 0; i < dim0; ++i) {
        out(i) = simd::dot(x.data(), yt.data() + (i * dim1), x.size());
    }

    return out;
}

template<typename T>
tensor<2, T> dot(const tensor<2, T>& x, const tensor<2, T>& y) {
    RACS_CHECK_COMPATIBLE_SHAPE(x, y)

    auto yt = transpose(y);

    auto dim0 = x.shape()[0];
    auto dim1 = yt.shape()[0];

    auto size0 = x.shape()[1];
    auto size1 = yt.shape()[1];

    tensor<2, T> out(tensor_shape({ dim0, dim1 }));

    for (size_t i = 0; i < dim0; ++i) {
        for (size_t j = 0; j < dim1; ++j) {
            out(i, j) = simd::dot(x.data() + (i * size0),
                                  yt.data() + (j * size1), size0);
        }
    }

    return out;
}

template float dot(const tensor<1, float>& x, const tensor<1, float>& y);
template tensor<1, float> dot(const tensor<1, float>& x, const tensor<2, float>& y);
template tensor<2, float> dot(const tensor<2, float>& x, const tensor<2, float>& y);

}