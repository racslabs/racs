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

#include "exponential.hpp"

namespace racs::math {

template<int NDIMS, typename T>
tensor<NDIMS, T> exp(const tensor<NDIMS, T> &x) {
    tensor<NDIMS, T> out(x.shape());
    simd::exp(x.data(), out.data(), x.size());
    return out;
}

template<int NDIMS, typename T>
tensor<NDIMS, T> power(const tensor<NDIMS, T> &x, const tensor<NDIMS, T> &y) {
    std::stringstream ss;

    if (x.shape() != y.shape()) {
        ss << x.shape() << " does not equal " << y.shape();
        throw runtime::zf100_exception(ss.str());
    }

    tensor<NDIMS, T> out(x.shape());
    simd::power(x.data(), y.data(), out.data(), x.size());
    return out;
}

template<int NDIMS, typename T1, typename T2 >
tensor<NDIMS, T2> power(T1 x, const tensor<NDIMS, T2> &y) {
    tensor<NDIMS, T2> _x(y.shape());
    _x.fill(x);

    simd::power(_x.data(), y.data(), _x.data(), y.size());
    return _x;
}

template<int NDIMS, typename T1, typename T2 >
tensor<NDIMS, T1> power(const tensor<NDIMS, T1> &x, T2 y) {
    tensor<NDIMS, T1> _y(x.shape());
    _y.fill(y);

    simd::power(x.data(), _y.data(), _y.data(), x.size());
    return _y;
}

template tensor<1, float> exp(const tensor<1, float> &x);
template tensor<2, float> exp(const tensor<2, float> &x);
template tensor<3, float> exp(const tensor<3, float> &x);

template tensor<1, float> power(const tensor<1, float> &x, const tensor<1, float> &y);
template tensor<2, float> power(const tensor<2, float> &x, const tensor<2, float> &y);
template tensor<3, float> power(const tensor<3, float> &x, const tensor<3, float> &y);

template tensor<1, float> power(const tensor<1, float> &x, float y);
template tensor<2, float> power(const tensor<2, float> &x, float y);
template tensor<3, float> power(const tensor<3, float> &x, float y);
template tensor<1, float> power(const tensor<1, float> &x, int y);
template tensor<2, float> power(const tensor<2, float> &x, int y);
template tensor<3, float> power(const tensor<3, float> &x, int y);

template tensor<1, float> power(float x, const tensor<1, float> &y);
template tensor<2, float> power(float x, const tensor<2, float> &y);
template tensor<3, float> power(float x, const tensor<3, float> &y);
template tensor<1, float> power(int x, const tensor<1, float> &y);
template tensor<2, float> power(int x, const tensor<2, float> &y);
template tensor<3, float> power(int x, const tensor<3, float> &y);

}

