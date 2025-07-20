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

#include "convolution.hpp"

namespace racs::fft {

template<typename T>
tensor<1, T> convolve(const tensor<1, T>& u, const tensor<1, T>& v) {
    auto _u = math::complex(u);
    auto _v = math::complex(v);
    auto out = convolve(_u, _v);

    return math::real(out);
}

template<typename T>
tensor<1, std::complex<T>> convolve(const tensor<1, std::complex<T>>& u,
                                    const tensor<1, std::complex<T>>& v) {
    auto k = v.size();
    auto _u = pad(u, 0, k - 1);

    return _convolve_base(_u, v);
}

template<typename T>
tensor<1, std::complex<T>> _convolve_base(const tensor<1, std::complex<T>>& u,
                                          const tensor<1, std::complex<T>>& v) {
    auto n = u.size();

    auto _u = pad(u, 0, nextpow2(n) - n);
    auto _v = broadcast_to(v, _u);

    auto out = ifft(fft(_u) * fft(_v));

    return slice(out, { 0 , n });
}

template<typename T>
tensor<1, T> correlate(const tensor<1, T>& u, const tensor<1, T>& v) {
    auto _u = math::complex(u);
    auto _v = math::complex(v);
    auto out = correlate(_u, _v);

    return math::real(out);
}

template<typename T>
tensor<1, std::complex<T>> correlate(const tensor<1, std::complex<T>>& u,
                                     const tensor<1, std::complex<T>>& v) {
    auto k = v.size();
    auto _u = pad(u, 0, k - 1);

    return _correlate_base(_u, v);
}

    template<typename T>
tensor<1, std::complex<T>> _correlate_base(const tensor<1, std::complex<T>>& u,
                                           const tensor<1, std::complex<T>>& v) {
        auto n = u.size();

        auto _u = pad(u, 0, nextpow2(n) - n);
        auto _v = broadcast_to(v, _u);

        auto out = ifft(fft(_u) * math::cconj(fft(_v)));

        return slice(out, { 0 , n });
}

template tensor<1, float> convolve(const tensor<1, float> &u, const tensor<1, float> &v);
template tensor<1, std::complex<float>> convolve(const tensor<1, std::complex<float>>& u,
                                                 const tensor<1, std::complex<float>>& v);
template tensor<1, std::complex<float>> _convolve_base(const tensor<1, std::complex<float>>& u,
                                                       const tensor<1, std::complex<float>>& v);

template tensor<1, float> correlate(const tensor<1, float> &u, const tensor<1, float> &v);
template tensor<1, std::complex<float>> correlate(const tensor<1, std::complex<float>>& u,
                                                  const tensor<1, std::complex<float>>& v);
template tensor<1, std::complex<float>> _correlate_base(const tensor<1, std::complex<float>>& u,
                                                        const tensor<1, std::complex<float>>& v);

}