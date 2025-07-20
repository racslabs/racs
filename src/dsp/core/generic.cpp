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

#include "generic.hpp"

namespace racs {

template<typename T>
tensor<1, T> zeros(size_t dim0) {
    return tensor<1, T>(tensor_shape({ dim0 }));
}

template<typename T>
tensor<2, T> zeros(size_t dim0, size_t dim1) {
    return tensor<2, T>(tensor_shape({ dim0, dim1 }));
}

template<typename T>
tensor<3, T> zeros(size_t dim0, size_t dim1, size_t dim2) {
    return tensor<3, T>(tensor_shape({ dim0, dim1, dim2 }));
}

template<typename T>
tensor<1, T> ones(size_t dim0) {
    auto out = tensor<1, T>(tensor_shape({ dim0 }));
    out.fill(1);

    return out;
}

template<typename T>
tensor<2, T> ones(size_t dim0, size_t dim1) {
    auto out = tensor<2, T>(tensor_shape({ dim0, dim1 }));
    out.fill(1);

    return out;
}

template<typename T>
tensor<3, T> ones(size_t dim0, size_t dim1, size_t dim2) {
    auto out = tensor<3, T>(tensor_shape({ dim0, dim1, dim2 }));
    out.fill(1);

    return out;
}

template<typename T>
tensor<1, T> pad(const tensor<1, T>& x, size_t left, size_t right, T c) {
    size_t dim0 = 0;

    if (x.shape().ndims() != 0) {
        dim0 = x.shape()[0];
    }

    auto size = dim0 + left + right;

    tensor<1, T> out(tensor_shape({ size }));
    out.fill(c);

    memcpy(out.data() + left, x.data(), sizeof(T) * dim0);

    return out;
}

template<typename T>
tensor<2, T> pad(const tensor<2, T>& x, size_t left, size_t right, T c) {
    auto dim0 = x.shape()[0];
    auto dim1 = x.shape()[1] + left + right;
    auto _dim1 = x.shape()[1];

    tensor<2, T> out(tensor_shape({ dim0, dim1 }));
    out.fill(c);


    for (size_t i = 0; i < dim0; ++i) {
        auto index = dim1 * i + left;
        auto _index = _dim1 * i;
        memcpy(out.data() + index, x.data() + _index , sizeof(T) * _dim1);
    }

    return out;
}

template<typename T>
tensor<1, std::complex<T>> pad(const tensor<1, std::complex<T>>& x, size_t left, size_t right, std::complex<T> c) {
    size_t dim0 = 0;

    if (x.shape().ndims() != 0) {
        dim0 = x.shape()[0];
    }

    auto size = dim0 + left + right;

    tensor<1, std::complex<T>> out(tensor_shape({ size }));
    out.fill(c);

    memcpy(out.data() + left, x.data(), sizeof(std::complex<T>) * dim0);

    return out;
}

template<typename T>
tensor<2, std::complex<T>> pad(const tensor<2, std::complex<T>>& x, size_t left, size_t right, std::complex<T> c) {
    auto dim0 = x.shape()[0];
    auto dim1 = x.shape()[1] + left + right;
    auto _dim1 = x.shape()[1];

    tensor<2, std::complex<T>> out(tensor_shape({ dim0, dim1 }));
    out.fill(c);

    for (size_t i = 0; i < dim0; ++i) {
        auto index = dim1 * i + left;
        auto _index = _dim1 * i;
        memcpy(out.data() + index, x.data() + _index , sizeof(std::complex<T>) * _dim1);
    }

    return out;
}

template<typename T>
tensor<1, T> dilate(const tensor<1, T>& x, size_t d) {
    auto dim0 = x.shape()[0];

    tensor<1, T> out(tensor_shape({ dim0 * d }));
    simd::dilate(x.data(), out.data(), dim0, d);

    return out;
}

template<typename T>
tensor<2, T> dilate(const tensor<2, T>& x, size_t d) {
    auto dim0 = x.shape()[0];
    auto dim1 = x.shape()[1];

    tensor<2, T> out(tensor_shape({ dim0, dim1 * d }));

    for (size_t i = 0; i < dim0 ; ++i) {
        auto index = i * dim1;
        simd::dilate(x.data() + index, out.data() + (index * d), dim1, d);
    }

    return out;
}

template<typename T>
tensor<3, T> dilate(const tensor<3, T>& x, size_t d) {
    auto dim0 = x.shape()[0];
    auto dim1 = x.shape()[1];
    auto dim2 = x.shape()[2];

    auto stride0 = x.strides()[0];
    auto stride1 = x.strides()[1];

    tensor<3, T> out(tensor_shape({ dim0, dim1, dim2 * d }));

    for (size_t i = 0; i < dim0; ++i) {
        for (size_t j = 0; j < dim1; ++j) {
            auto index0 = stride0 * i + stride1 * j;
            auto index1 = stride0 * i * d + stride1 * j * d;
            simd::dilate(x.data() + index0, out.data() + index1, dim2, d);
        }
    }

    return out;
}

template<typename T>
tensor<1, T> reverse(const tensor<1, T>& x) {
    tensor<1, T> out(x.shape());
    simd::rev(x.data(), out.data(), x.size());
    return out;
}

template<typename T>
tensor<1, std::complex<T>> reverse(const tensor<1, std::complex<T>>& x) {
    tensor<1, std::complex<T>> out(x.shape());
    simd::crev(x.data(), out.data(), x.size());
    return out;
}

template<typename T>
tensor<1, T> linspace(T start, T stop, size_t num) {
    T step = (stop - start) / (num - 1);

    tensor<1, T> out(tensor_shape({ num }));
    std::fill_n(out.data(), num, step);
    out(0) = start;

    simd::cumsum(out.data(), out.data(), num);
    return out;
}

template<typename T>
tensor<1, T> arange(T start, T stop, T step) {

    size_t size = ((stop - start + 1) / step);

    tensor<1, T> out(tensor_shape({ size }));
    std::fill_n(out.data(), size, step);
    out(0) = start;

    simd::cumsum(out.data(), out.data(), size);

    return out;
}

template<typename T>
tensor<1, T> concatenate(const tensor<1, T>& a, const tensor<1, T>& b) {
    auto dim0 = a.size() + b.size();

    tensor<1, T> out(tensor_shape({ dim0 }));
    memcpy(out.data(), a.data(), sizeof(T) * a.size());
    memcpy(out.data() + a.size(), b.data(), sizeof(T) * b.size());

    return out;
}

template<typename T>
tensor<1, std::complex<T>> concatenate(const tensor<1, std::complex<T>>& a,
                                       const tensor<1, std::complex<T>>& b) {
    auto dim0 = a.size() + b.size();

    tensor<1, std::complex<T>> out(tensor_shape({ dim0 }));
    memcpy(out.data(), a.data(), sizeof(std::complex<T>) * a.size());
    memcpy(out.data() + a.size(), b.data(), sizeof(std::complex<T>) * b.size());

    return out;
}

template<typename T>
tensor<1, T> interleaved(const tensor<2, T>& x) {
    auto dim1 = x.shape()[1];
    tensor<1, T> out(tensor_shape({ x.size() }));

    simd::interleaved(x.data(), x.data() + dim1, out.data(), dim1);
    return out;
}

template<typename T>
tensor<2, T> planar(const tensor<1, T>& x) {
    auto dim1 = x.shape()[0] / 2;

    tensor<2, T> out(tensor_shape({ 2, dim1 }));
    simd::planar(x.data(), out.data(), x.size());

    return out;
}

template tensor<1, float> zeros(size_t dim0);
template tensor<2, float> zeros(size_t dim0, size_t dim1);
template tensor<3, float> zeros(size_t dim0, size_t dim1, size_t dim2);

template tensor<1, float> ones(size_t dim0);
template tensor<2, float> ones(size_t dim0, size_t dim1);
template tensor<3, float> ones(size_t dim0, size_t dim1, size_t dim2);

template tensor<1, float> pad(const tensor<1, float>& x, size_t left, size_t right, float c);
template tensor<2, float> pad(const tensor<2, float>& x, size_t left, size_t right, float c);

template tensor<1, std::complex<float>> pad(const tensor<1, std::complex<float>>& x, size_t left, size_t right, std::complex<float> c);
template tensor<2, std::complex<float>> pad(const tensor<2, std::complex<float>>& x, size_t left, size_t right, std::complex<float> c);

template tensor<1, float> dilate(const tensor<1, float>& x, size_t d);
template tensor<2, float> dilate(const tensor<2, float>& x, size_t d);
template tensor<3, float> dilate(const tensor<3, float>& x, size_t d);

template tensor<1, float> linspace(float start, float stop, size_t num);
template tensor<1, float> arange(float start, float stop, float step);

template tensor<1, float> reverse(const tensor<1, float>& x);

template tensor<1, std::complex<float>> reverse(const tensor<1, std::complex<float>>& x);

template tensor<1, float> concatenate(const tensor<1, float>& a, const tensor<1, float>& b);
template tensor<1, std::complex<float>> concatenate(const tensor<1, std::complex<float>>& a,
                                                    const tensor<1, std::complex<float>>& b);

template tensor<1, float> interleaved(const tensor<2, float>& x);
template tensor<2, float> planar(const tensor<1, float>& x);

}
