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

#include "bitcast.hpp"

namespace racs {

template<typename T>
tensor<1, T> bitcast(const tensor<1, float>& in) {
    if constexpr (std::is_same<std::complex<float>, T>::value) {
        auto storage = reinterpret_cast<std::complex<float> *>(in.data());
        if (in.shape().ndims() != 0) {
            tensor_shape shape({ in.shape()[0] / 2 });
            return tensor<1, std::complex<float>>(shape, storage, in.count());
        }

        return {};
    }
}

template<typename T>
tensor<1, T> bitcast(const tensor<1, std::complex<float>>& in) {
    if constexpr (std::is_same<float, T>::value) {
        auto storage = reinterpret_cast<float *>(in.data());

        if (in.shape().ndims() != 0) {
            tensor_shape shape({ in.shape()[0] * 2 });
            return tensor<1, float>(shape, storage, in.count());
        }

        return {};
    }
}

template tensor<1, std::complex<float>> bitcast<std::complex<float>>(const tensor<1, float>& in);
template tensor<1, float> bitcast<float>(const tensor<1, std::complex<float>>& in);

}
