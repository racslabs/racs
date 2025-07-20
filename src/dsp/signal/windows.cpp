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

#include "windows.hpp"

namespace racs::signal {

template<typename T>
tensor<1, T> hanning(size_t size) {
    auto n = arange<T>(0, size - 1, 1);
    auto x = (math::pi2<T> * n) / (size- 1);
    return 0.5 * (1 - math::cos(x));
}

template<typename T>
tensor<1, T> hamming(size_t size) {
    auto n = arange<T>(0, size - 1, 1);
    auto x = (math::pi2<T> * n) / (size - 1);
    return 0.54 - 0.46 * math::cos(x);
}

template<typename T>
tensor<1, T> blackman(size_t size) {
    auto n = arange<T>(0, size - 1, 1);
    auto x = (math::pi2<T> * n) / size;
    return 0.42 - 0.5 * math::cos(x) + 0.08 * math::cos(2 * x);
}

template<typename T>
tensor<1, T> bartlett(size_t size) {
    auto n = arange<T>(0, size - 1, 1);

    auto c0 = 2 / T(size - 1);
    auto c1 = T(size - 1) / 2;

    return c0 * (c1 - math::abs(n - c1));
}

template<typename T>
tensor<1, T> kaiser(size_t size, T beta) {
    auto n = arange<T>(0, size - 1, 1);
    auto x = (((2 * n) / size) - 1);

    auto num = special::i0(beta * math::sqrt(1 - (x * x)));
    auto dem = simd::i0(beta);

    return num / dem;
}

template tensor<1, float> hanning(size_t size);
template tensor<1, float> hamming(size_t size);
template tensor<1, float> blackman(size_t size);
template tensor<1, float> bartlett(size_t size);
template tensor<1, float> kaiser(size_t size, float beta);

}