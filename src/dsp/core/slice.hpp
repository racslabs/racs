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

#ifndef RACS_SLICE_HPP
#define RACS_SLICE_HPP

#include "tensor.hpp"
#include "../runtime/exceptions.hpp"

namespace racs {

#define RACS_CHECK_RANGE(x, start, end) \
    std::stringstream ss;                \
    if (start >= end) {                  \
        ss << "start position must be less than " << end; \
        throw runtime::zf102_exception(ss.str());          \
    }                                    \
    if (start >= x.shape()[0]) {         \
        ss << "start position " << start << "is out of range"; \
        throw runtime::zf102_exception(ss.str());          \
    }                                    \
    if (end > x.shape()[0]) {            \
        ss << "end position " << end << "is out of range";\
        throw runtime::zf102_exception(ss.str());          \
    }

#define RACS_CHECK_INDEX(x, index) \
    std::stringstream ss;           \
    if (index >= x.shape()[0]) {    \
        ss << "index " << index << " is out of range"; \
        throw runtime::zf102_exception(ss.str());      \
    }

template<typename T>
tensor<1, T> slice(const tensor<1, T>& x, std::pair<size_t, size_t> range);

template<typename T>
tensor<2, T> slice(const tensor<2, T>& x, std::pair<size_t, size_t> range);

template<typename T>
tensor<3, T> slice(const tensor<3, T>& x, std::pair<size_t, size_t> range);

template<typename T>
tensor<4, T> slice(const tensor<4, T>& x, std::pair<size_t, size_t> range);

template<typename T>
T slice(const tensor<1, T>& x, size_t index);

template<typename T>
tensor<1, T> slice(const tensor<2, T>& x, size_t index);

template<typename T>
tensor<2, T> slice(const tensor<3, T>& x, size_t index);

template<typename T>
tensor<3, T> slice(const tensor<4, T>& x, size_t index);

template<typename T>
tensor<1, std::complex<T>> slice(const tensor<1, std::complex<T>>& x, std::pair<size_t, size_t> range);

template<typename T>
tensor<2, std::complex<T>> slice(const tensor<2, std::complex<T>>& x, std::pair<size_t, size_t> range);

template<typename T>
tensor<3, std::complex<T>> slice(const tensor<3, std::complex<T>>& x, std::pair<size_t, size_t> range);

template<typename T>
tensor<4, std::complex<T>> slice(const tensor<4, std::complex<T>>& x, std::pair<size_t, size_t> range);

template<typename T>
std::complex<T> slice(const tensor<1, std::complex<T>>& x, size_t index);

template<typename T>
tensor<1, std::complex<T>> slice(const tensor<2, std::complex<T>>& x, size_t index);

template<typename T>
tensor<2, std::complex<T>> slice(const tensor<3, std::complex<T>>& x, size_t index);

template<typename T>
tensor<3, std::complex<T>> slice(const tensor<4, std::complex<T>>& x, size_t index);

}

#endif //RACS_SLICE_HPP
