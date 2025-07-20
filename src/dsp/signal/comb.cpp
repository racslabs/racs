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

#include "comb.hpp"

namespace racs::signal {

template<typename T>
tensor<1, T> ffcomb(const tensor<1, T>& in, size_t n, T g) {
    tensor<1, T> out(in.shape());
    tensor<1, T> _in = pad(in, n);

    simd::ffcomb(in.data(), _in.data(), out.data(), g, in.size());
    return out;
}

template<typename T>
tensor<1, T> fbcomb(const tensor<1, T>& in, size_t n, T g) {
    tensor<1, T> out(in.shape());
    simd::fbcomb(in.data(), out.data(), g, in.size(), n);
    return out;
}

template<typename T>
tensor<1, T> apcomb(const tensor<1, T>& in, size_t n, T g) {
    tensor<1, T> out(in.shape());
    tensor<1, T> _in = pad(in, n);

    simd::apcomb(in.data(), _in.data(), out.data(), g, in.size(), n);
    return out;
}

template tensor<1, float> ffcomb(const tensor<1, float>& in, size_t n, float g);
template tensor<1, float> fbcomb(const tensor<1, float>& in, size_t n, float g);
template tensor<1, float> apcomb(const tensor<1, float>& in, size_t n, float g);

}