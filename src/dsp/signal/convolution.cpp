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

#include "convolution.hpp"

namespace racs::signal {

template<typename T>
tensor<1, T> convolve(const tensor<1, T>& u, const tensor<1, T>& v) {
    auto k = v.size();
    auto _u = pad(u, k - 1, k - 1);
    auto _v = reverse(v);
    auto size = _u.size() - _v.size() + 1;

    tensor<1, T> out(tensor_shape({ size }));
    simd::convolve(_u.data(), _v.data(), out.data(), _u.size(), _v.size());

    return out;
}

template<typename T>
tensor<1, T> correlate(const tensor<1, T>& u, const tensor<1, T>& v) {
    auto k = v.size();
    auto _u = pad(u, k - 1, k - 1);
    auto size = _u.size() - v.size() + 1;

    tensor<1, T> out(tensor_shape({ size }));
    simd::convolve(_u.data(), v.data(), out.data(), _u.size(), v.size());

    return out;
}

template tensor<1, float> convolve(const tensor<1, float> &u, const tensor<1, float> &v);
template tensor<1, float> correlate(const tensor<1, float>& u, const tensor<1, float>& v);

}