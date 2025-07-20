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

#include "bessel.hpp"

namespace racs::special {

template<typename T>
tensor<1, T> i0(const tensor<1, T>& x) {
    tensor<1, T> out(x.shape());
    simd::i0(x.data(), out.data(), x.size());

    return out;
}

template<typename T>
tensor<1, T> i0e(const tensor<1, T>& x) {
    return math::exp(-1 * math::abs(x)) * i0(x);
}

template<typename T>
tensor<1, T> k0(const tensor<1, T>& x) {
    tensor<1, T> out(x.shape());
    simd::k0(x.data(), out.data(), x.size());

    return out;
}

template<typename T>
tensor<1, T> k0e(const tensor<1, T>& x) {
    return math::exp(x) * k0(x);
}

template tensor<1, float> i0(const tensor<1, float>& x);

template tensor<1, float> i0e(const tensor<1, float>& x);

template tensor<1, float> k0(const tensor<1, float>& x);

template tensor<1, float> k0e(const tensor<1, float>& x);

}
