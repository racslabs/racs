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

#include "fir_design.hpp"

namespace racs::signal {

template<typename T>
tensor<1, T> fir_lp(const tensor<1, T>& window, T fc, T fs) {
    size_t taps = window.size();

    T wc = (fc / fs) * math::pi2<T>;
    T alpha = 0.5 * T(taps - 1);

    T s = 2 * (fc / fs);

    auto n = arange<T>(0, taps - 1, 1) - alpha;
    auto hd = s * math::sinc(n * wc);

    return window * hd;
}

template<typename T>
tensor<1, T> fir_hp(const tensor<1, T>& window, T fc, T fs) {
    size_t taps = window.size();

    T wc = (fc / fs) * math::pi2<T>;
    T alpha = 0.5 * T(taps - 1);

    T s = 2 * (fc / fs);

    auto n = arange<T>(0, taps - 1, 1) - alpha;
    auto hd = 1 - s * math::sinc(n * wc);

    return window * hd;
}

template<typename T>
tensor<1, T> fir_bp(const tensor<1, T>& window, T f0, T f1, T fs) {
    size_t taps = window.size();

    T w0 = (f0 / fs) * math::pi2<T>;
    T w1 = (f1 / fs) * math::pi2<T>;
    T alpha = 0.5 * T(taps - 1);

    T s0 = 2 * (f0 / fs);
    T s1 = 2 * (f1 / fs);

    auto n = arange<T>(0, taps - 1, 1) - alpha;

    auto hd_l = math::sinc(n * w0);
    auto hd_r = math::sinc(n * w1);
    auto hd = s1 * hd_r - s0 * hd_l;

    return window * hd;
}

template<typename T>
tensor<1, T> fir_bs(const tensor<1, T>& window, T f0, T f1, T fs) {
    size_t taps = window.size();

    T w0 = (f0 / fs) * math::pi2<T>;
    T w1 = (f1 / fs) * math::pi2<T>;
    T alpha = 0.5 * T(taps - 1);

    T s0 = 2 * (f0 / fs);
    T s1 = 2 * (f1 / fs);

    auto n = arange<T>(0, taps - 1, 1) - alpha;

    auto hd_l = math::sinc(n * w0);
    auto hd_r = math::sinc(n * w1);
    auto hd = 1 - (s1 * hd_r - s0 * hd_l);

    return window * hd;
}

template<typename T>
tensor<1, T> fir(const tensor<1, T>& window,T fc, BTYPE type, T fs) {
    return fir<T>(window, fc, 0, type, fs);
}

template<typename T>
tensor<1, T> fir(const tensor<1, T>& window, T f0, T f1, BTYPE type, T fs) {
    switch (type) {
        case BTYPE::LOWPASS:
            return fir_lp<T>(window, f0, fs);
        case BTYPE::HIGHPASS:
            return fir_hp<T>(window, f0, fs);
        case BTYPE::BANDPASS:
            return fir_bp<T>(window, f0, f1, fs);
        case BTYPE::BANDSTOP:
            return fir_bs<T>(window, f0, f1, fs);
    }
}

template tensor<1, float> fir_lp(const tensor<1, float>& window, float fc, float fs);

template tensor<1, float> fir_hp(const tensor<1, float>& window, float fc, float fs);

template tensor<1, float> fir_bp(const tensor<1, float>& window, float f0, float f1, float fs);

template tensor<1, float> fir_bs(const tensor<1, float>& window, float f0, float f1, float fs);

template tensor<1, float> fir(const tensor<1, float>& window, float fc, BTYPE type, float fs);

template tensor<1, float> fir(const tensor<1, float>& window, float f0, float f1, BTYPE type, float fs);

}
