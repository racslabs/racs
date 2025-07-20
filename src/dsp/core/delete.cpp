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

#include "delete.hpp"

namespace racs {

template<typename T>
tensor<1, T> del(const tensor<1, T>& x, size_t idx) {
    auto dim0 = x.shape()[0];
    auto _idx = idx + 1;

    tensor<1, T> out(tensor_shape({ dim0 - 1 }));
    memcpy(out.data(), x.data(), sizeof(T) * idx);
    memcpy(out.data() + idx, x.data() + _idx, sizeof(T) * (dim0 - _idx));

    return out;
}

template<typename T>
tensor<1, std::complex<T>> del(const tensor<1, std::complex<T>>& x, size_t idx) {
    auto dim0 = x.shape()[0];
    auto _idx = idx + 1;

    tensor<1, std::complex<T>> out(tensor_shape({ dim0 - 1 }));
    memcpy(out.data(), x.data(), sizeof(std::complex<T>) * idx);
    memcpy(out.data() + idx, x.data() + _idx, sizeof(std::complex<T>) * (dim0 - _idx));

    return out;
}

template tensor<1, float> del(const tensor<1, float>& x, size_t idx);

template tensor<1, std::complex<float>> del(const tensor<1, std::complex<float>>& x, size_t idx);

}