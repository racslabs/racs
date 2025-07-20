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

#ifndef RACS_IIR_DESIGN_HPP
#define RACS_IIR_DESIGN_HPP

#include "../core/op.hpp"
#include "../core/put.hpp"
#include "../core/tensor.hpp"
#include "../core/delete.hpp"
#include "../math/complex.hpp"
#include "../math/trigonometric.hpp"
#include "../math/polynomial.hpp"
#include "biquad.hpp"

namespace racs::signal {

template<typename T>
struct zpk {
    tensor<1, std::complex<T>> z;
    tensor<1, std::complex<T>> p;
    T k;
};

enum class BTYPE : unsigned {
    LOWPASS,
    HIGHPASS,
    BANDPASS,
    BANDSTOP
};

template<typename T>
int rdegree(const zpk<T>& zpk);

template<typename T>
T prewrap(T wn, T fs);

template<typename T>
zpk<T> bilinear(const zpk<T>& zpk, T fs);

template<typename T>
zpk<T> buttap(int n);

template<typename T>
zpk<T> cheb1ap(int n, T rp);

template<typename T>
zpk<T> cheb2ap(int n, T rp);

template<typename T>
zpk<T> lp2lp_zpk(const zpk<T>& zpk, T wo = T(1.0));

template<typename T>
zpk<T> lp2hp_zpk(const zpk<T>& zpk, T wo = T(1.0));

template<typename T>
zpk<T> lp2bp_zpk(const zpk<T>& zpk, T wo = T(1.0), T bw = T(1.0));

template<typename T>
zpk<T> lp2bs_zpk(const zpk<T>& zpk, T wo = T(1.0), T bw = T(1.0));

template<typename T>
tensor<2, T> zpk2tf(const zpk<T>& zpk);

template<typename T>
bool _isreal(std::complex<T> x);

template<typename T>
size_t _nearest_idx(const tensor<1, std::complex<T>>& fro, std::complex<T> to);

template<typename T>
tensor<1, T> _zpksos(const zpk<T>& zpk);

template<typename T>
tensor<2, T> zpk2sos(const zpk<T>& zpk);

template<typename T>
zpk<T> butter(int n, T wo, BTYPE btype = BTYPE::LOWPASS, T fs = 44100);

template<typename T>
zpk<T> butter(int n, T w0, T w1, BTYPE btype = BTYPE::LOWPASS, T fs = 44100);

template<typename T>
zpk<T> cheby1(int n, T wo, T rp, BTYPE btype = BTYPE::LOWPASS, T fs = 44100);

template<typename T>
zpk<T> cheby1(int n, T w0, T w1, T rp, BTYPE btype = BTYPE::LOWPASS, T fs = 44100);

template<typename T>
zpk<T> cheby2(int n, T wo, T rp, BTYPE btype = BTYPE::LOWPASS, T fs = 44100);

template<typename T>
zpk<T> cheby2(int n, T w0, T w1, T rp, BTYPE btype = BTYPE::LOWPASS, T fs = 44100);

template<typename T>
zpk<T> iirfilter(const zpk<T>& zpk, T w0, T b1, BTYPE btype = BTYPE::LOWPASS, T fs = 44100);

template<typename T>
tensor<1, T> sosfilt(const tensor<2, T>& sos, const tensor<1, T>& x);

template<typename T>
tensor<2, T> sosfilt(const tensor<2, T>& sos, const tensor<2, T>& x);

template<typename T>
tensor<2, T> _sosfilt_parallel(const tensor<2, T>& sos, const tensor<2, T>& x);

template<typename T>
tensor<2, T> _sosfilt_parallel_multithreaded(const tensor<2, T>& sos, const tensor<2, T>& x);
}


#endif //RACS_IIR_DESIGN_HPP
