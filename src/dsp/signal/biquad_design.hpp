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

#ifndef RACS_BIQUAD_DESIGN_HPP
#define RACS_BIQUAD_DESIGN_HPP

#include "biquad.hpp"
#include "../math/constants.hpp"

namespace racs::signal {

template<typename T>
tensor<1, T> allpass(T fc, T q, T fs = 44100);

template<typename T>
tensor<1, T> lowpass(T fc, T q, T fs = 44100);

template<typename T>
tensor<1, T> highpass(T fc, T q, T fs = 44100);

template<typename T>
tensor<1, T> bandpass(T fc, T bw, T fs = 44100);

template<typename T>
tensor<1, T> bandstop(T fc, T bw, T fs = 44100);

template<typename T>
tensor<1, T> lowshelf(T fc, T g, T fs = 44100);

template<typename T>
tensor<1, T> highshelf(T fc, T g, T fs = 44100);

template<typename T>
tensor<1, T> peaking(T fc, T q, T g, T fs = 44100);

}

#endif //RACS_BIQUAD_DESIGN_HPP
