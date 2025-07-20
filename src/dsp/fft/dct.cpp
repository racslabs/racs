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

#include "dct.hpp"

namespace racs::fft {

template<typename T>
tensor<1, T> dct(const tensor<1, T> &in) {
    auto n = in.size();
    auto pad_in = pad(in, 0, nextpow2(n) - n);

    return _dct_base(pad_in);
}

template<typename T>
tensor<1, T> idct(const tensor<1, T> &in) {
    auto n = in.size();
    auto pad_in = pad(in, 0, nextpow2(n) - n);

    return _idct_base(pad_in);
}

template<typename T>
tensor<1, T> _dct_even_odd(const tensor<1, T>& in) {
    tensor<1, T> out(in.shape());
    simd::dct_even_odd(in.data(), out.data(), in.size());
    return out;
}

template<typename T>
tensor<1, T> _idct_even_odd(const tensor<1, T>& in) {
    tensor<1, T> out(in.shape());
    simd::idct_even_odd(in.data(), out.data(), in.size());
    return out;
}

template<typename T>
tensor<1, T> _dct_base(const tensor<1, T>& in) {
    auto _in = _dct_even_odd(in);
    auto dft = _fft_base(math::complex(_in));

    T m = _in.size();

    auto n = linspace<T>(0, m - 1, m);
    n = (n * math::pi<T>) / (2 * m);

    auto k = std::complex<T>(0, -1) * n;
    auto out = math::cexp(k) * dft;

    return math::real(out) * 2;
}

template<typename T>
tensor<1, T> _idct_base(const tensor<1, T>& in) {
    T m = in.size();

    auto n = linspace<T>(0, m - 1, m);
    n = (n * math::pi<T>) / (2 * m);

    auto k = std::complex<T>(0, 1) * n;
    auto _in = math::complex(in) * math::cexp(k);

    auto idft = _ifft_base(_in);
    auto out = _idct_even_odd(math::real(idft));

    auto offset = (in(0) / (2 * m));

    return out - offset;
}

template tensor<1, float> _dct_even_odd(const tensor<1, float> &in);
template tensor<1, float> _idct_even_odd(const tensor<1, float> &in);
template tensor<1, float> _dct_base(const tensor<1, float> &in);
template tensor<1, float> _idct_base(const tensor<1, float>& in);
template tensor<1, float> dct(const tensor<1, float> &in);
template tensor<1, float> idct(const tensor<1, float> &in);

}