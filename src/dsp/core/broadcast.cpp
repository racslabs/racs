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

#include "broadcast.hpp"

namespace racs {

template<typename T>
tensor<1, T> broadcast_to(const tensor<1, T>& src, const tensor<1, T>& dst, T value) {
    RACS_CHECK_BROADCAST_SHAPE(src, dst)

    tensor<1, T> out(dst.shape());
    out.fill(value);

    memcpy(out.data(), src.data(), sizeof(T) * src.size());
    return out;
}

template<typename T>
tensor<2, T> broadcast_to(const tensor<1, T>& src, const tensor<2, T>& dst, T value) {
    RACS_CHECK_BROADCAST_SHAPE(src, dst)

    tensor<2, T> out(dst.shape());
    out.fill(value);

    auto dim0 = dst.shape()[0];
    auto col0 = src.shape()[0];
    auto col1 = dst.shape()[1];

    for (size_t i = 0; i < dim0; ++i) {
        memcpy(out.data() + (i * col1), src.data(), sizeof(T) * col0);
    }

    return out;
}

template<typename T>
tensor<2, T> broadcast_to(const tensor<2, T>& src, const tensor<2, T>& dst, T value) {
    RACS_CHECK_BROADCAST_SHAPE(src, dst)

    tensor<2, T> out(dst.shape());
    out.fill(value);

    auto dim0 = src.shape()[0];
    auto col0 = src.shape()[1];
    auto col1 = dst.shape()[1];

    for (size_t i = 0; i < dim0; ++i) {
        memcpy(out.data() + (i * col1), src.data() + (i * col0),
               sizeof(T) * col0);
    }

    return out;
}

template<typename T>
tensor<3, T> broadcast_to(const tensor<1, T>& src, const tensor<3, T>& dst, T value) {
    RACS_CHECK_BROADCAST_SHAPE(src, dst)

    tensor<3, T> out(dst.shape());
    out.fill(value);

    auto dim0 = dst.shape()[0];
    auto dim1 = dst.shape()[1];

    auto col0 = src.shape()[0];
    auto col1 = dst.shape()[2];

    for (size_t i = 0; i < dim0; ++i) {

        auto index = i * dim1 * dim0;

        for (size_t j = 0; j < dim1; ++j) {
            auto row1 = index + j * col1;
            memcpy(out.data() + row1, src.data(), sizeof(T) * col0);
        }
    }

    return out;
}

template<typename T>
tensor<3, T> broadcast_to(const tensor<2, T>& src, const tensor<3, T>& dst, T value) {
    RACS_CHECK_BROADCAST_SHAPE(src, dst)

    tensor<3, T> out(dst.shape());
    out.fill(value);

    auto dim0 = dst.shape()[0];
    auto dim1 = dst.shape()[1];

    auto col0 = src.shape()[1];
    auto col1 = dst.shape()[2];

    for (size_t i = 0; i < dim0; ++i) {

        auto index = i * dim1 * dim0;

        for (size_t j = 0; j < dim1; ++j) {
            auto row0 = j * col0;
            auto row1 = index + j * col1;

            memcpy(out.data() + row1, src.data() + row0, sizeof(T) * col0);
        }
    }

    return out;
}

template<typename T>
tensor<3, T> broadcast_to(const tensor<3, T>& src, const tensor<3, T>& dst, T value) {
    RACS_CHECK_BROADCAST_SHAPE(src, dst)

    tensor<3, T> out(dst.shape());
    out.fill(value);
    
    auto dim0 = dst.shape()[0];
    auto dim1 = dst.shape()[1];
    auto dim2 = src.shape()[2];

    auto strides0 = src.strides();
    auto strides1 = out.strides();

    for (size_t i = 0; i < dim0; ++i) {
        for (size_t j = 0; j < dim1; ++j) {
            auto index0 = strides0[0] * i + strides0[1] * j;
            auto index1 = strides1[0] * i + strides1[1] * j;

            memcpy(out.data() + index1, src.data() + index0, sizeof(T) * dim2);
        }
    }

    return out;
}

template<typename T>
tensor<2, std::complex<T>> broadcast_to(const tensor<1, std::complex<T>>& src,
                                        const tensor<2, std::complex<T>>& dst,
                                        std::complex<T> value) {
    RACS_CHECK_BROADCAST_SHAPE(src, dst)

    tensor<2, std::complex<T>> out(dst.shape());
    out.fill(value);

    auto dim0 = dst.shape()[0];
    auto col0 = src.shape()[0];
    auto col1 = dst.shape()[1];

    for (size_t i = 0; i < dim0; ++i) {
        memcpy(out.data() + (i * col1), src.data(), sizeof(std::complex<T>) * col0);
    }

    return out;
}

template<typename T>
tensor<1, std::complex<T>> broadcast_to(const tensor<1, std::complex<T>>& src,
                                        const tensor<1, std::complex<T>>& dst,
                                        std::complex<T> value) {
    RACS_CHECK_BROADCAST_SHAPE(src, dst)

    tensor<1, std::complex<T>> out(dst.shape());
    out.fill(value);

    memcpy(out.data(), src.data(), sizeof(std::complex<T>) * src.size());
    return out;
}

template<typename T>
tensor<2, std::complex<T>> broadcast_to(const tensor<3, std::complex<T>> &src,
                                        const tensor<3, std::complex<T>> &dst,
                                        std::complex<T> value) {
    RACS_CHECK_BROADCAST_SHAPE(src, dst)

    tensor<2, std::complex<T>> out(dst.shape());
    out.fill(value);

    auto dim0 = dst.shape()[0];
    auto col0 = src.shape()[0];
    auto col1 = dst.shape()[1];

    for (size_t i = 0; i < dim0; ++i) {
        memcpy(out.data() + (i * col1), src.data(), sizeof(std::complex<T>) * col0);
    }

    return out;
}

template<typename T>
tensor<2, std::complex<T>> broadcast_to(const tensor<2, std::complex<T>>& src,
                                        const tensor<2, std::complex<T>>& dst,
                                        std::complex<T> value) {
    RACS_CHECK_BROADCAST_SHAPE(src, dst)

    tensor<2, std::complex<T>> out(dst.shape());
    out.fill(value);

    auto dim0 = src.shape()[0];
    auto col0 = src.shape()[1];
    auto col1 = dst.shape()[1];

    for (size_t i = 0; i < dim0; ++i) {
        memcpy(out.data() + (i * col1), src.data() + (i * col0),
               sizeof(std::complex<T>) * col0);
    }

    return out;
}

template<typename T>
tensor<3, std::complex<T>> broadcast_to(const tensor<1, std::complex<T>>& src,
                                        const tensor<3, std::complex<T>>& dst,
                                        std::complex<T> value) {
    RACS_CHECK_BROADCAST_SHAPE(src, dst)

    tensor<3, std::complex<T>> out(dst.shape());
    out.fill(value);

    auto dim0 = dst.shape()[0];
    auto dim1 = dst.shape()[1];

    auto col0 = src.shape()[0];
    auto col1 = dst.shape()[2];

    for (size_t i = 0; i < dim0; ++i) {

        auto index = i * dim1 * dim0;

        for (size_t j = 0; j < dim1; ++j) {
            auto row1 = index + j * col1;
            memcpy(out.data() + row1, src.data(), sizeof(std::complex<T>) * col0);
        }
    }

    return out;
}

template<typename T>
tensor<3, std::complex<T>> broadcast_to(const tensor<2, std::complex<T>>& src,
                                        const tensor<3, std::complex<T>>& dst,
                                        std::complex<T> value) {
    RACS_CHECK_BROADCAST_SHAPE(src, dst)

    tensor<3, std::complex<T>> out(dst.shape());
    out.fill(value);

    auto dim0 = dst.shape()[0];
    auto dim1 = dst.shape()[1];

    auto col0 = src.shape()[1];
    auto col1 = dst.shape()[2];

    for (size_t i = 0; i < dim0; ++i) {

        auto index = i * dim1 * dim0;

        for (size_t j = 0; j < dim1; ++j) {
            auto row0 = j * col0;
            auto row1 = index + j * col1;

            memcpy(out.data() + row1, src.data() + row0, sizeof(std::complex<T>) * col0);
        }
    }

    return out;
}

template<typename T>
tensor<3, std::complex<T>> broadcast_to(const tensor<3, std::complex<T>>& src,
                                        const tensor<3, std::complex<T>>& dst,
                                        std::complex<T> value) {
    RACS_CHECK_BROADCAST_SHAPE(src, dst)

    tensor<3, std::complex<T>> out(dst.shape());
    out.fill(value);

    auto dim0 = dst.shape()[0];
    auto dim1 = dst.shape()[1];
    auto dim2 = src.shape()[2];

    auto strides0 = src.strides();
    auto strides1 = out.strides();

    for (size_t i = 0; i < dim0; ++i) {
        for (size_t j = 0; j < dim1; ++j) {
            auto index0 = strides0[0] * i + strides0[1] * j;
            auto index1 = strides1[0] * i + strides1[1] * j;

            memcpy(out.data() + index1, src.data() + index0, sizeof(std::complex<T>) * dim2);
        }
    }

    return out;
}
template tensor<1, float> broadcast_to(const tensor<1, float>& src, const tensor<1, float>& dst, float value);
template tensor<2, float> broadcast_to(const tensor<1, float>& src, const tensor<2, float>& dst, float value);
template tensor<2, float> broadcast_to(const tensor<2, float>& src, const tensor<2, float>& dst, float value);
template tensor<3, float> broadcast_to(const tensor<1, float>& src, const tensor<3, float>& dst, float value);
template tensor<3, float> broadcast_to(const tensor<2, float>& src, const tensor<3, float>& dst, float value);
template tensor<3, float> broadcast_to(const tensor<3, float>& src, const tensor<3, float>& dst, float value);

template tensor<1, std::complex<float>> broadcast_to(const tensor<1, std::complex<float>>& src, const tensor<1, std::complex<float>>& dst, std::complex<float> value);
template tensor<2, std::complex<float>> broadcast_to(const tensor<1, std::complex<float>>& src, const tensor<2, std::complex<float>>& dst, std::complex<float> value);
template tensor<2, std::complex<float>> broadcast_to(const tensor<2, std::complex<float>>& src, const tensor<2, std::complex<float>>& dst, std::complex<float> value);
template tensor<3, std::complex<float>> broadcast_to(const tensor<1, std::complex<float>>& src, const tensor<3, std::complex<float>>& dst, std::complex<float> value);
template tensor<3, std::complex<float>> broadcast_to(const tensor<2, std::complex<float>>& src, const tensor<3, std::complex<float>>& dst, std::complex<float> value);
template tensor<3, std::complex<float>> broadcast_to(const tensor<3, std::complex<float>>& src, const tensor<3, std::complex<float>>& dst, std::complex<float> value);
}