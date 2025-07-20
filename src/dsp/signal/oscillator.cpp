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

#include "oscillator.hpp"

namespace racs::signal {

template<typename T>
tensor<1, T> oscillator(const tensor<2, T>& x, T fs, WAVEFORM waveform) {
    #ifdef RACS_MULTITHREADED
        return _oscillator_parallel_multithreaded(x, fs, waveform);
    #else
        return _oscillator_multithreaded(x, fs, waveform);
    #endif
}

template<typename T>
tensor<1, T> oscillator(const tensor<1, T>& x, T fs, WAVEFORM waveform) {
    T ts = 1 / fs;

    auto deltas = x * ts * math::pi2<T>;
    auto phase = math::cumsum(deltas) % math::pi2<T>;

    return get_waveform(phase, deltas, waveform);
}

template<typename T>
tensor<1, T> get_waveform(const tensor<1, T>& x,
                          const tensor<1, T>& deltas,
                          WAVEFORM waveform) {
    switch (waveform) {
        case WAVEFORM::SINE:
            return cos(x);
        case WAVEFORM::SAWTOOTH:
            return sawtooth(x, deltas);
        case WAVEFORM::SQUARE:
            return square(x, deltas);
        case WAVEFORM::TRIANGLE:
            return triangle(x, deltas);
    }
}


template<typename T>
tensor<1, T> sin(const tensor<1, T>& x) {
    tensor<1, T> out(x.shape());
    simd::sin(x.data(), out.data(), x.size());
    return out;
}

template<typename T>
tensor<1, T> cos(const tensor<1, T>& x) {
    tensor<1, T> out(x.shape());
    simd::cos(x.data(), out.data(), x.size());
    return out;
}

template<typename T>
tensor<1, T> leaky_integrator(const tensor<1, T>& x, const tensor<1, T>& deltas) {
    tensor<1, T> out(x.shape());
    simd::leaky_integrator(x.data(), deltas.data(), out.data(), x.size());
    return out;
}

template<typename T>
tensor<1, T> sawtooth(const tensor<1, T>& x, const tensor<1, T>& deltas) {
    tensor<1, T> out(x.shape());
    simd::sawtooth(x.data(), deltas.data(), out.data(), x.size());
    return out;
}

template<typename T>
tensor<1, T> square(const tensor<1, T>& x, const tensor<1, T>& deltas) {
    tensor<1, T> out(x.shape());
    simd::square(x.data(), deltas.data(), out.data(), x.size());
    return out;
}

template<typename T>
tensor<1, T> triangle(const tensor<1, T>& x, const tensor<1, T>& deltas) {
    auto square_wave = square(x, deltas);
    return leaky_integrator(square_wave, deltas);
}

template tensor<1, float> sin(const tensor<1, float>& x);

template tensor<1, float> cos(const tensor<1, float>& x);

template tensor<1, float> leaky_integrator(const tensor<1, float>& x,
                                           const tensor<1, float>& deltas);

template tensor<1, float> sawtooth(const tensor<1, float>& x,
                                   const tensor<1, float>& deltas);

template tensor<1, float> square(const tensor<1, float>& x,
                                 const tensor<1, float>& deltas);

template tensor<1, float> triangle(const tensor<1, float>& x,
                                   const tensor<1, float>& deltas);

template tensor<1, float> get_waveform(const tensor<1, float> &x,
                                       const tensor<1, float> &deltas,
                                       WAVEFORM waveform);

template tensor<1, float> oscillator(const tensor<1, float>& x,
                                     float fs,
                                     WAVEFORM waveform);

}
