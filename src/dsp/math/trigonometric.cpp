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

#include "trigonometric.hpp"

namespace racs::math {

template<int NDIMS, typename T>
tensor<NDIMS, T> cos(const tensor<NDIMS, T> &x) {
    tensor<NDIMS, T> out(x.shape());
    simd::cos(x.data(), out.data(), x.size());
    return out;
}

template<int NDIMS, typename T>
tensor<NDIMS, T> sin(const tensor<NDIMS, T> &x) {
    tensor<NDIMS, T> out(x.shape());
    simd::sin(x.data(), out.data(), x.size());
    return out;
}

template<int NDIMS, typename T>
tensor<NDIMS, T> sinc(const tensor<NDIMS, T> &x) {
    tensor<NDIMS, T> out(x.shape());
    simd::sinc(x.data(), out.data(), x.size());
    return out;
}

template<int NDIMS, typename T>
tensor<NDIMS, T> tan(const tensor<NDIMS, T> &x) {
    tensor<NDIMS, T> out(x.shape());
    simd::tan(x.data(), out.data(), x.size());
    return out;
}

template<int NDIMS, typename T>
tensor<NDIMS, T> cot(const tensor<NDIMS, T> &x) {
    tensor<NDIMS, T> out(x.shape());
    simd::cot(x.data(), out.data(), x.size());
    return out;
}

template<int NDIMS, typename T>
tensor<NDIMS, T> sec(const tensor<NDIMS, T> &x) {
    tensor<NDIMS, T> out(x.shape());
    simd::sec(x.data(), out.data(), x.size());
    return out;
}

template<int NDIMS, typename T>
tensor<NDIMS, T> csc(const tensor<NDIMS, T> &x) {
    tensor<NDIMS, T> out(x.shape());
    simd::csc(x.data(), out.data(), x.size());
    return out;
}

template tensor<1, float> cos(const tensor<1, float> &x);
template tensor<2, float> cos(const tensor<2, float> &x);
template tensor<3, float> cos(const tensor<3, float> &x);

template tensor<1, float> sin(const tensor<1, float> &x);
template tensor<2, float> sin(const tensor<2, float> &x);
template tensor<3, float> sin(const tensor<3, float> &x);

template tensor<1, float> tan(const tensor<1, float> &x);
template tensor<2, float> tan(const tensor<2, float> &x);
template tensor<3, float> tan(const tensor<3, float> &x);

template tensor<1, float> cot(const tensor<1, float> &x);
template tensor<2, float> cot(const tensor<2, float> &x);
template tensor<3, float> cot(const tensor<3, float> &x);

template tensor<1, float> sec(const tensor<1, float> &x);
template tensor<2, float> sec(const tensor<2, float> &x);
template tensor<3, float> sec(const tensor<3, float> &x);

template tensor<1, float> csc(const tensor<1, float> &x);
template tensor<2, float> csc(const tensor<2, float> &x);
template tensor<3, float> csc(const tensor<3, float> &x);

template tensor<1, float> sinc(const tensor<1, float> &x);
template tensor<2, float> sinc(const tensor<2, float> &x);
template tensor<3, float> sinc(const tensor<3, float> &x);

}