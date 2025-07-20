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

#ifndef RACS_OSCILLATOR_HPP
#define RACS_OSCILLATOR_HPP

#include "../core/tensor.hpp"
#include "../core/op.hpp"
#include "../core/slice.hpp"
#include "../math/cumsum.hpp"
#include "../core/generic.hpp"
#include "../simd/waveforms.hpp"
#include "../simd/trigonometric.hpp"
#include "../runtime/session.hpp"

namespace racs::signal {

enum class WAVEFORM : unsigned {
    SINE,
    SAWTOOTH,
    TRIANGLE,
    SQUARE
};

template<typename T>
tensor<1, T> sin(const tensor<1, T>& x);

template<typename T>
tensor<1, T> cos(const tensor<1, T>& x);

template<typename T>
tensor<1, T> leaky_integrator(const tensor<1, T>& x, const tensor<1, T>& deltas);

template<typename T>
tensor<1, T> sawtooth(const tensor<1, T>& x, const tensor<1, T>& deltas);

template<typename T>
tensor<1, T> square(const tensor<1, T>& x, const tensor<1, T>& deltas);

template<typename T>
tensor<1, T> triangle(const tensor<1, T>& x, const tensor<1, T>& deltas);

template<typename T>
tensor<1, T> get_waveform(const tensor<1, T>& x, const tensor<1, T>& deltas, WAVEFORM waveform);

template<typename T>
tensor<1, T> oscillator(const tensor<1, T>& x, T fs = 44100, WAVEFORM waveform = WAVEFORM::SINE);

}


#endif //RACS_OSCILLATOR_HPP
