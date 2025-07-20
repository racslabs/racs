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

#include "reshape.hpp"

namespace racs {

template<int NDIMS0, int NDIMS1, typename T>
tensor<NDIMS1, T> reshape(const tensor<NDIMS0, T>& x, const tensor_shape& shape) {
    RACS_CHECK_SHAPE_COMPATIBLE(x.shape(), shape)
    tensor<NDIMS1, T> out(shape, x.data(), x.count());
    return out;
}

template<int NDIMS0, int NDIMS1, typename T>
tensor<NDIMS1, std::complex<T>> reshape(const tensor<NDIMS0, std::complex<T>>& x, const tensor_shape& shape) {
    RACS_CHECK_SHAPE_COMPATIBLE(x.shape(), shape)
    tensor<NDIMS1, std::complex<T>> out(shape, x.data(), x.count());
    return out;
}

template tensor<2, float> reshape(const tensor<1, float>& x, const tensor_shape& shape);
template tensor<3, float> reshape(const tensor<1, float>& x, const tensor_shape& shape);
template tensor<1, float> reshape(const tensor<2, float>& x, const tensor_shape& shape);
template tensor<2, float> reshape(const tensor<2, float>& x, const tensor_shape& shape);
template tensor<3, float> reshape(const tensor<2, float>& x, const tensor_shape& shape);
template tensor<1, float> reshape(const tensor<3, float>& x, const tensor_shape& shape);
template tensor<2, float> reshape(const tensor<3, float>& x, const tensor_shape& shape);
template tensor<3, float> reshape(const tensor<3, float>& x, const tensor_shape& shape);

template tensor<2, std::complex<float>> reshape(const tensor<1, std::complex<float>>& x, const tensor_shape& shape);
template tensor<3, std::complex<float>> reshape(const tensor<1, std::complex<float>>& x, const tensor_shape& shape);
template tensor<1, std::complex<float>> reshape(const tensor<2, std::complex<float>>& x, const tensor_shape& shape);
template tensor<2, std::complex<float>> reshape(const tensor<2, std::complex<float>>& x, const tensor_shape& shape);
template tensor<3, std::complex<float>> reshape(const tensor<2, std::complex<float>>& x, const tensor_shape& shape);
template tensor<1, std::complex<float>> reshape(const tensor<3, std::complex<float>>& x, const tensor_shape& shape);
template tensor<2, std::complex<float>> reshape(const tensor<3, std::complex<float>>& x, const tensor_shape& shape);
template tensor<3, std::complex<float>> reshape(const tensor<3, std::complex<float>>& x, const tensor_shape& shape);

}
