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

#include "slice.hpp"

namespace racs {

template<typename T>
tensor<1, T> slice(const tensor<1, T>& x, std::pair<size_t, size_t> range) {
    auto start = range.first;
    auto end = range.second;
    auto dim0 = end - start;

    RACS_CHECK_RANGE(x, start, end)

    tensor<1, T> out(tensor_shape({ dim0 }), x.data() + start, x.count());

    return out;
}

template<typename T>
tensor<2, T> slice(const tensor<2, T>& x, std::pair<size_t, size_t> range) {
    auto stride0 = x.strides()[0];

    auto start = range.first;
    auto end = range.second;

    RACS_CHECK_RANGE(x, start, end)

    auto _start = start * stride0;

    auto dim0 = end - start;
    auto dim1 = x.shape()[1];

    tensor<2, T> out(tensor_shape({ dim0, dim1 }), x.data() + _start, x.count());

    return out;
}

template<typename T>
tensor<3, T> slice(const tensor<3, T>& x, std::pair<size_t, size_t> range) {
    auto stride0 = x.strides()[0];

    auto start = range.first;
    auto end = range.second;

    RACS_CHECK_RANGE(x, start, end)

    auto _start = start * stride0;

    auto dim0 = end - start;
    auto dim1 = x.shape()[1];
    auto dim2 = x.shape()[2];

    tensor<3, T> out(tensor_shape({ dim0, dim1, dim2 }), x.data() + _start, x.count());

    return out;
}

template<typename T>
tensor<4, T> slice(const tensor<4, T>& x, std::pair<size_t, size_t> range) {
    auto stride0 = x.strides()[0];

    auto start = range.first;
    auto end = range.second;

    RACS_CHECK_RANGE(x, start, end)

    auto _start = start * stride0;

    auto dim0 = end - start;
    auto dim1 = x.shape()[1];
    auto dim2 = x.shape()[2];
    auto dim3 = x.shape()[3];

    tensor<4, T> out(tensor_shape({ dim0, dim1, dim2, dim3 }), x.data() + _start, x.count());

    return out;
}

template<typename T>
T slice(const tensor<1, T>& x, size_t index) {
    RACS_CHECK_INDEX(x, index)
    return x(index);
}

template<typename T>
tensor<1, T> slice(const tensor<2, T>& x, size_t index) {
    RACS_CHECK_INDEX(x, index)

    auto dim0 = x.strides()[0];

    tensor<1, T> out(tensor_shape({ dim0 }), x.data() + (dim0 * index), x.count());

    return out;
}

template<typename T>
tensor<2, T> slice(const tensor<3, T>& x, size_t index) {
    RACS_CHECK_INDEX(x, index)

    auto dim0 = x.strides()[0];
    auto dim1 = x.shape()[1];
    auto dim2 = x.shape()[2];

    tensor<2, T> out(tensor_shape({ dim1, dim2 }), x.data() + (dim0 * index), x.count());

    return out;
}

template<typename T>
tensor<3, T> slice(const tensor<4, T>& x, size_t index) {
    RACS_CHECK_INDEX(x, index)

    auto dim0 = x.strides()[0];
    auto dim1 = x.shape()[1];
    auto dim2 = x.shape()[2];
    auto dim3 = x.shape()[3];

    tensor<3, T> out(tensor_shape({ dim1, dim2, dim3 }), x.data() + (dim0 * index), x.count());

    return out;
}

template<typename T>
tensor<1, std::complex<T>> slice(const tensor<1, std::complex<T>>& x, std::pair<size_t, size_t> range) {
    auto start = range.first;
    auto end = range.second;
    auto dim0 = end - start;

    RACS_CHECK_RANGE(x, start, end)

    tensor<1, std::complex<T>> out(tensor_shape({ dim0 }), x.data() + start, x.count());

    return out;
}

template<typename T>
tensor<2, std::complex<T>> slice(const tensor<2, std::complex<T>>& x, std::pair<size_t, size_t> range) {
    auto stride0 = x.strides()[0];

    auto start = range.first;
    auto end = range.second;

    RACS_CHECK_RANGE(x, start, end)

    auto _start = start * stride0;

    auto dim0 = end - start;
    auto dim1 = x.shape()[1];

    tensor<2, std::complex<T>> out(tensor_shape({ dim0, dim1 }), x.data() + _start, x.count());

    return out;
}

template<typename T>
tensor<3, std::complex<T>> slice(const tensor<3, std::complex<T>>& x, std::pair<size_t, size_t> range) {
    auto stride0 = x.strides()[0];

    auto start = range.first;
    auto end = range.second;

    RACS_CHECK_RANGE(x, start, end)

    auto _start = start * stride0;

    auto dim0 = end - start;
    auto dim1 = x.shape()[1];
    auto dim2 = x.shape()[2];

    tensor<3, std::complex<T>> out(tensor_shape({ dim0, dim1, dim2 }), x.data() + _start, x.count());

    return out;
}

template<typename T>
tensor<4, std::complex<T>> slice(const tensor<4, std::complex<T>>& x, std::pair<size_t, size_t> range) {
    auto stride0 = x.strides()[0];

    auto start = range.first;
    auto end = range.second;

    RACS_CHECK_RANGE(x, start, end)

    auto _start = start * stride0;

    auto dim0 = end - start;
    auto dim1 = x.shape()[1];
    auto dim2 = x.shape()[2];
    auto dim3 = x.shape()[3];

    tensor<4, std::complex<T>> out(tensor_shape({ dim0, dim1, dim2, dim3 }), x.data() + _start, x.count());

    return out;
}

template<typename T>
std::complex<T> slice(const tensor<1, std::complex<T>>& x, size_t index) {
    RACS_CHECK_INDEX(x, index)
    return x(index);
}

template<typename T>
tensor<1, std::complex<T>> slice(const tensor<2, std::complex<T>>& x, size_t index) {
    RACS_CHECK_INDEX(x, index)

    auto dim0 = x.strides()[0];

    tensor<1, std::complex<T>> out(tensor_shape({ dim0 }), x.data() + (dim0 * index), x.count());

    return out;
}

template<typename T>
tensor<2, std::complex<T>> slice(const tensor<3, std::complex<T>>& x, size_t index) {
    RACS_CHECK_INDEX(x, index)

    auto dim0 = x.strides()[0];
    auto dim1 = x.shape()[1];
    auto dim2 = x.shape()[2];

    tensor<2, std::complex<T>> out(tensor_shape({ dim1, dim2 }), x.data() + (dim0 * index), x.count());

    return out;
}

template<typename T>
tensor<3, std::complex<T>> slice(const tensor<4, std::complex<T>>& x, size_t index) {
    RACS_CHECK_INDEX(x, index)

    auto dim0 = x.strides()[0];
    auto dim1 = x.shape()[1];
    auto dim2 = x.shape()[2];
    auto dim3 = x.shape()[3];

    tensor<3, std::complex<T>> out(tensor_shape({ dim1, dim2, dim3 }), x.data() + (dim0 * index), x.count());

    return out;
}

template tensor<1, float> slice(const tensor<1, float>& x, std::pair<size_t, size_t> range);
template tensor<2, float> slice(const tensor<2, float>& x, std::pair<size_t, size_t> range);
template tensor<3, float> slice(const tensor<3, float>& x, std::pair<size_t, size_t> range);
template tensor<4, float> slice(const tensor<4, float>& x, std::pair<size_t, size_t> range);

template float slice(const tensor<1, float>& x, size_t index);

template tensor<1, float> slice(const tensor<2, float>& x, size_t index);
template tensor<2, float> slice(const tensor<3, float>& x, size_t index);
template tensor<3, float> slice(const tensor<4, float>& x, size_t index);

template tensor<1, std::complex<float>> slice(const tensor<1, std::complex<float>>& x, std::pair<size_t, size_t> range);
template tensor<2, std::complex<float>> slice(const tensor<2, std::complex<float>>& x, std::pair<size_t, size_t> range);
template tensor<3, std::complex<float>> slice(const tensor<3, std::complex<float>>& x, std::pair<size_t, size_t> range);
template tensor<4, std::complex<float>> slice(const tensor<4, std::complex<float>>& x, std::pair<size_t, size_t> range);

template std::complex<float> slice(const tensor<1, std::complex<float>>& x, size_t index);

template tensor<1, std::complex<float>> slice(const tensor<2, std::complex<float>>& x, size_t index);
template tensor<2, std::complex<float>> slice(const tensor<3, std::complex<float>>& x, size_t index);
template tensor<3, std::complex<float>> slice(const tensor<4, std::complex<float>>& x, size_t index);

}