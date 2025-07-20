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

#include "dwt.hpp"

namespace racs::dwt {

template<typename T>
tensor<2, T> dwt(const tensor<1, T>& in, WAVELET name) {
    wavelet<T> wavelet(name);

    auto filter_bank = wavelet.filter_bank();

    auto lo_D = slice(filter_bank, 0);
    auto hi_D = slice(filter_bank, 1);

    return _dwt_base(in, lo_D, hi_D);
}

template<typename T>
tensor<1, T> idwt(const tensor<2, T>& in, WAVELET name) {
    wavelet<T> wavelet(name);

    auto cA = slice(in, 0);
    auto cD = slice(in, 1);

    auto filter_bank = wavelet.filter_bank();

    auto lo_R = slice(filter_bank, 2);
    auto hi_R = slice(filter_bank, 3);

    return _idwt_base(cA, cD, lo_R, hi_R);
}

template<typename T>
tensor<1, T> _downsample(const tensor<1, T>& x) {
    auto c_x = bitcast<std::complex<T>>(x);
    return math::imag(c_x);
}

template<typename T>
tensor<1, T> _upsample(const tensor<1, T>& x) {
    auto c_x = math::complex(x);
    return bitcast<T>(c_x);
}

template<typename T>
tensor<2, T> _dwt_base(const tensor<1, T>& in,
                       const tensor<1, T>& lo_D,
                       const tensor<1, T>& hi_D) {
    #ifdef RACS_MULTITHREADED
        return _dwt_filterbank_multithreaded(in, lo_D, hi_D);
    #else
        return _dwt_filterbank(in, lo_D, hi_D);
    #endif
}

template<typename T>
tensor<1, T> _idwt_base(const tensor<1, T>& cA,
                        const tensor<1, T>& cD,
                        const tensor<1, T>& lo_R,
                        const tensor<1, T>& hi_R) {

    #ifdef RACS_MULTITHREADED
        auto out = _idwt_filterbank_multithreaded(cA, cD, lo_R, hi_R);
    #else
        auto out = _idwt_filterbank(cA, cD, lo_R, hi_R);
    #endif

    auto _cA = slice(out, 0);
    auto _cD = slice(out, 1);

    return _cA + _cD;
}

template<typename T>
tensor<2, T> _dwt_filterbank(const tensor<1, T> &in,
                             const tensor<1, T> &lo_D,
                             const tensor<1, T> &hi_D) {

    auto cA = fft::convolve(in, lo_D);
    auto d_cA = _downsample(cA);

    auto cD = fft::convolve(in, hi_D);
    auto d_cD = _downsample(cD);

    auto n = cA.size();

    tensor<2, T> out(tensor_shape({ 2, n }));
    put(out, 0, d_cA);
    put(out, 1, d_cD);

    return out;
}

template<typename T>
tensor<2, T> _idwt_filterbank(const tensor<1, T>& cA,
                              const tensor<1, T>& cD,
                              const tensor<1, T>& lo_R,
                              const tensor<1, T>& hi_R) {

    auto u_cA = _upsample(cA);
    auto u_cD = _upsample(cD);

    auto _cA = fft::convolve(u_cA, lo_R);
    auto _cD = fft::convolve(u_cD, hi_R);

    auto n = _cD.size();

    tensor<2, T> out(tensor_shape({ 2, n }));
    put(out, 0, _cA);
    put(out, 1, _cD);

    return out;
}

template<typename T>
tensor<2, T> _dwt_filterbank_multithreaded(const tensor<1, T> &in,
                                           const tensor<1, T> &lo_D,
                                           const tensor<1, T> &hi_D) {

    runtime::session<tensor<1, T>> session;
    runtime::task_list<tensor<1, T>> tasks;

    tasks.push_back([&, in, lo_D] {
        auto cA = fft::convolve(in, lo_D);
        auto d_cA = _downsample(cA);

        return d_cA;
    });

    tasks.push_back([&, in, hi_D] {
        auto cD = fft::convolve(in, hi_D);
        auto d_cD = _downsample(cD);

        return d_cD;
    });

    auto callback = session.execute_tasks(std::move(tasks));

    auto cA = callback[0].get();
    auto cD = callback[1].get();

    auto n = cA.size();

    tensor<2, T> out(tensor_shape({ 2, n }));
    put(out, 0, cA);
    put(out, 1, cD);

    return out;
}

template<typename T>
tensor<2, T> _idwt_filterbank_multithreaded(const tensor<1, T>& cA,
                                            const tensor<1, T>& cD,
                                            const tensor<1, T>& lo_R,
                                            const tensor<1, T>& hi_R) {

    runtime::session<tensor<1, T>> session;
    runtime::task_list<tensor<1, T>> tasks;

    tasks.push_back([&, cA, lo_R] {
        auto u_cA = _upsample(cA);
        auto _cA = fft::convolve(u_cA, lo_R);

        auto n = _cA.size();

        return slice(_cA, { 0, n - 1 });
    });

    tasks.push_back([&, cD, hi_R] {
        auto u_cD = _upsample(cD);
        auto _cD = fft::convolve(u_cD, hi_R);

        auto n = _cD.size();

        return slice(_cD, { 0, n - 1 });
    });

    auto callback = session.execute_tasks(std::move(tasks));

    auto _cA = callback[0].get();
    auto _cD = callback[1].get();

    auto n = _cA.size();

    tensor<2, T> out(tensor_shape({ 2, n }));
    put(out, 0, _cA);
    put(out, 1, _cD);

    return out;
}

template tensor<1, float> _downsample(const tensor<1, float>& x);

template tensor<1, float> _upsample(const tensor<1, float>& x);

template tensor<2, float> dwt(const tensor<1, float>& in, WAVELET name);

template tensor<1, float> idwt(const tensor<2, float>& in, WAVELET name);

template tensor<2, float> _dwt_base(const tensor<1, float> &in,
                                    const tensor<1, float> &lo_D,
                                    const tensor<1, float> &hi_D);

template tensor<1, float> _idwt_base(const tensor<1, float>& cA,
                                     const tensor<1, float>& cD,
                                     const tensor<1, float>& lo_R,
                                     const tensor<1, float>& hi_R);

template tensor<2, float> _dwt_filterbank(const tensor<1, float> &in,
                                          const tensor<1, float> &lo_D,
                                          const tensor<1, float> &hi_D);

template tensor<2, float> _dwt_filterbank_multithreaded(const tensor<1, float> &in,
                                                        const tensor<1, float> &lo_D,
                                                        const tensor<1, float> &hi_D);

template tensor<2, float> _idwt_filterbank(const tensor<1, float>& cA,
                                           const tensor<1, float>& cD,
                                           const tensor<1, float>& lo_R,
                                           const tensor<1, float>& hi_R);

template tensor<2, float> _idwt_filterbank_multithreaded(const tensor<1, float>& cA,
                                                         const tensor<1, float>& cD,
                                                         const tensor<1, float>& lo_R,
                                                         const tensor<1, float>& hi_R);

}