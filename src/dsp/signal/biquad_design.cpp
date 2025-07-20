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

#include "biquad_design.hpp"

namespace racs::signal {

template<typename T>
tensor<1, T> allpass(T fc, T q, T fs) {
    auto theta = math::pi<T> * (q / fs);
    auto alpha = (tan(theta) - 1) / (tan(theta) + 1);

    auto omega = math::pi2<T> * (fc / fs);
    auto beta = -cos(omega);

    T a0 = -alpha;
    T a1 = beta * (1 - alpha);

    T b1 = a1;
    T b2 = -alpha;

    return { a0, a1, 1, 1, b1, b2 };
}

template<typename T>
tensor<1, T> lowpass(T fc, T q, T fs) {
    auto d = 1 / q;
    auto omega = math::pi2<T> * (fc / fs);

    auto beta_n = 1 - (d / 2) * std::sin(omega);
    auto beta_d = 1 + (d / 2) * std::sin(omega);
    auto beta = 0.5 * beta_n / beta_d;

    auto gamma = (0.5 + beta) * std::cos(omega);

    T a0 = (0.5 + beta - gamma) / 2;
    T a1 = 0.5 + beta - gamma;
    T a2 = a0;

    T b1 = -2 * gamma;
    T b2 = 2 * beta;

    return { a0, a1, a2, 1, b1, b2 };
}

template<typename T>
tensor<1, T> highpass(T fc, T q, T fs) {
    auto d = 1 / q;
    auto omega = math::pi2<T> * (fc / fs);

    auto beta_n = 1 - (d / 2) * std::sin(omega);
    auto beta_d = 1 + (d / 2) * std::sin(omega);
    auto beta = 0.5 * beta_n / beta_d;

    auto gamma = (0.5 + beta) * std::cos(omega);

    T a0 = (0.5 + beta + gamma) / 2;
    T a1 = -0.5 - beta - gamma;
    T a2 = a0;

    T b1 = -2 * gamma;
    T b2 = 2 * beta;

    return { a0, a1, a2, 1, b1, b2 };
}

template<typename T>
tensor<1, T> bandpass(T fc, T bw, T fs) {
    auto q = 1 / bw;
    auto omega = math::pi2<T> * (fc / fs);

    auto beta_n = 1 - std::tan(omega / 2 * q);
    auto beta_d = 1 + std::tan(omega / 2 * q);
    auto beta = 0.5 * (beta_n / beta_d);

    auto gamma = (0.5 + beta) * std::cos(omega);

    T a0 = 0.5 - beta;
    T a2 = -0.5 + beta;

    T b1 = -2 * gamma;
    T b2 = 2 * beta;

    return { a0, 0, a2, 1, b1, b2 };
}

template<typename T>
tensor<1, T> bandstop(T fc, T bw, T fs) {
    auto q = 1 / bw;
    auto omega = math::pi2<T> * (fc / fs);

    auto beta_n = 1 - std::tan(omega / 2 * q);
    auto beta_d = 1 + std::tan(omega / 2 * q);
    auto beta = 0.5 * (beta_n / beta_d);

    auto gamma = (0.5 + beta) * std::cos(omega);

    T a0 = 0.5 + beta;
    T a1 = -2 * gamma;
    T a2 = a0;

    T b1 = a1;
    T b2 = 2 * beta;

    return { a0, a1, a2, 1, b1, b2 };
}

template<typename T>
tensor<1, T> lowshelf(T fc, T g, T fs) {
    auto omega = math::pi2<T> * (fc / fs);

    auto mu = std::pow(10, g / 20);
    auto beta = 4 / (1 + mu);

    auto delta = beta * std::tan(omega / 2);
    auto gamma = (1 - delta) / (1 + delta);

    T a0 = (1 - gamma) / 2;
    T a1 = a0;

    T b1 = -gamma;
    T b2 = 0.0;

    T c0 = mu - 1;

    return { a0, a1, 0, 1, b1, b2, c0, 1 };
}

template<typename T>
tensor<1, T> highshelf(T fc, T g, T fs) {
    auto omega = math::pi2<T> * (fc / fs);

    auto mu = std::pow(10, g / 20);
    auto beta = (1 + mu) / 4;

    auto delta = beta * std::tan(omega / 2);
    auto gamma = (1 - delta) / (1 + delta);

    T a0 = (1 + gamma) / 2;
    T a1 = -a0;

    T b1 = -gamma;
    T c0 = mu - 1;

    return { a0, a1, 0, 1, b1, 0, c0, 1 };
}

template<typename T>
tensor<1, T> peaking(T fc, T q, T g, T fs) {
    auto omega = math::pi2<T> * (fc / fs);

    auto mu = std::pow(10, g / 20);
    auto zeta = 4 / (1 + mu);

    auto beta_n = 1 - zeta * std::tan(omega / 2 * q);
    auto beta_d = 1 + zeta * std::tan(omega / 2 * q);
    auto beta = 0.5 * beta_n / beta_d;

    auto gamma = (0.5 + beta) * std::cos(omega);

    T a0 = 0.5 - beta;
    T a2 = -a0;

    T b1 = -gamma;
    T b2 = 2 * beta;

    T c0 = mu - 1;

    return { a0, 0, a2, 1, b1, b2, c0, 1 };
}

template tensor<1, float> allpass(float fc, float q, float fs);

template tensor<1, float> lowpass(float fc, float q, float fs);

template tensor<1, float> highpass(float fc, float q, float fs);

template tensor<1, float> bandpass(float fc, float bw, float fs);

template tensor<1, float> bandstop(float fc, float bw, float fs);

template tensor<1, float> lowshelf(float fc, float g, float fs);

template tensor<1, float> highshelf(float fc, float g, float fs);

template tensor<1, float> peaking(float fc, float q, float g, float fs);

}
