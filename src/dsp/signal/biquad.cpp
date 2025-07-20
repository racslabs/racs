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

#include "biquad.hpp"

namespace racs::signal {

template<typename T>
tensor<1, T> biquad(const tensor<1, T>& x, const tensor<1, T>& params) {
    tensor<1, T> out(x.shape());
    simd::biquad(x.data(), out.data(), params.data(), x.size());

    if (params.size() == 6) return out;
    return params[7] * x + params[6] * out;
}

template<typename T>
tensor<2, T> biquad(const tensor<2, T>& x, const tensor<1, T>& params) {
    tensor<2, T> out(x.shape());

    auto dim1 = x.shape()[1];
    simd::biquad(x.data(), out.data(), params.data(), dim1);
    simd::biquad(x.data() + dim1, out.data() + dim1, params.data(), dim1);

    if (params.size() == 6) return out;
    return params[7] * x + params[6] * out;
}

template tensor<1, float> biquad(const tensor<1, float>& x, const tensor<1, float>& params);
template tensor<2, float> biquad(const tensor<2, float>& x, const tensor<1, float>& params);

}
