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

#include "polynomial.hpp"

namespace racs::math {

template<typename T>
tensor<1, T> polyval(const tensor<1, T>& p, const tensor<1, T>& x) {
    tensor<1, T> out(x.shape());
    simd::polyval(p.data(), x.data(), out.data(), p.size(), x.size());
    return out;
}

template<typename T>
tensor<1, std::complex<T>> polyval(const tensor<1, std::complex<T>>& p,
                                   const tensor<1, std::complex<T>>& x) {
    tensor<1, std::complex<T>> out(x.shape());
    simd::polyval(p.data(), x.data(), out.data(), p.size(), x.size());
    return out;
}

template<typename T>
tensor<1, std::complex<T>> poly(const tensor<1, std::complex<T>>& z) {
    auto n = z.size();

    if (n > 0) {
        tensor<1, std::complex<T>> p = { 1, -z(0) };

        for (int i = 1; i < z.size(); ++i) {
            p = fft::convolve(p, { 1, -z(i) });
        }

        return p;
    }

    return { 1 };
}

template<typename T>
tensor<1, std::complex<T>> roots(const tensor<1, T>& p) {
    auto _p = p * std::complex<T>(1, 0);
    return roots(_p);
}

template<typename T>
tensor<1, std::complex<T>> roots(const tensor<1, std::complex<T>>& p) {
    size_t n = p.size() - 1;

    auto a0_re = p(n).real();
    auto a0_im = p(n).imag();
    auto a0_mag = std::sqrt(a0_re * a0_re + a0_im * a0_im);

    auto an_re = p(0).real();
    auto an_im = p(0).imag();
    auto an_mag = std::sqrt(an_re * an_re + an_im * an_im);

    auto r = std::pow(a0_mag / an_mag, 1 / T(n));
    auto c = pi2<T> / n;

    auto x = unityroots(n, r, c);

    for (size_t i = 0; i < 10; ++i) {
        auto q = ones<T>(n) * std::complex<T>(1, 0);

        RACS_UNROLL(4)
        for (size_t j = 0; j < n; ++j) {
            tensor<1, std::complex<T>> s(x.shape());
            s.fill(x(j));

            auto t = x - s;
            t(j) = 1;
            q = q * t;
        }

        x = x - (polyval(p, x) / q);
    }

    return x;
}

template<typename T>
tensor<1, std::complex<T>> unityroots(size_t n, T r, T c) {
    auto theta = pi2<T> / n;
    auto k = linspace<T>(0, n - 1, n);
    auto phi = k * theta + c;

    return cexp(phi * std::complex<T>(0, 1)) * r;
}

template tensor<1, float> polyval(const tensor<1, float>& p, const tensor<1, float>& x);

template tensor<1, std::complex<float>> polyval(const tensor<1, std::complex<float>>& p,
                                                const tensor<1, std::complex<float>>& x);

template tensor<1, std::complex<float>> poly(const tensor<1, std::complex<float>>& z);

template tensor<1, std::complex<float>> roots(const tensor<1, float>& p);

template tensor<1, std::complex<float>> roots(const tensor<1, std::complex<float>>& p);

template tensor<1, std::complex<float>> unityroots(size_t n, float r, float c);

}