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

#include "random.hpp"

namespace racs::stats {

#ifdef __AVX2__
template<typename T>
tensor<1, T> random(size_t dim) {
    tensor<1, T> out(tensor_shape({ dim }));
    simd::xoshiro128(out.data(), out.size());
    return out;
}

template<typename T>
tensor<2, T> random(size_t dim0 , size_t dim1) {
    tensor<2, T> out(tensor_shape({ dim0, dim1 }));
    simd::xoshiro128(out.data(), out.size());
    return out;
}

template tensor<1, float> random(size_t dim);
template tensor<2, float> random(size_t dim0 , size_t dim1);
#else
    #warning "Random requires AVX2"
#endif
}
