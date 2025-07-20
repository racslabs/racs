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

#ifndef RACS_WINDOWS_HPP
#define RACS_WINDOWS_HPP

#include "../math/trigonometric.hpp"
#include "../math/sqrt.hpp"
#include "../special/bessel.hpp"
#include "../core/broadcast.hpp"
#include "../core/generic.hpp"
#include "../core/op.hpp"

namespace racs::signal {

template<typename T>
tensor<1, T> hanning(size_t size);

template<typename T>
tensor<1, T> hamming(size_t size);

template<typename T>
tensor<1, T> blackman(size_t size);

template<typename T>
tensor<1, T> bartlett(size_t size);

template<typename T>
tensor<1, T> kaiser(size_t size, T beta);

}

#endif //RACS_WINDOWS_HPP
