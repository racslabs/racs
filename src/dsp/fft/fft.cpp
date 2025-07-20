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

#include "fft.hpp"

namespace racs::fft {

template<typename T>
tensor<1, std::complex<T>>  fft(const tensor<1, std::complex<T>>& in) {
    return _fft_base(in);
}

template<typename T>
tensor<2, std::complex<T>> fft(const tensor<2, std::complex<T>>& in) {
    #ifdef RACS_MULTITHREADED
        return _fft_parallel_multithreaded(in);
    #else
        return _fft_parallel(in);
    #endif
}

template<typename T>
tensor<1, std::complex<T>> ifft(const tensor<1, std::complex<T>>& in) {
    return _ifft_base(in);
}

template<typename T>
tensor<2, std::complex<T>> ifft(const tensor<2, std::complex<T>>& in) {
    #ifdef RACS_MULTITHREADED
        return _ifft_parallel_multithreaded(in);
    #else
        return _ifft_parallel(in);
    #endif
}

template<typename T>
tensor<1, std::complex<T>> rfft(const tensor<1, T>& in) {
    auto c_in = math::complex(in);
    auto c_out = fft(c_in);
    auto n = c_out.size();

    size_t m = (n / 2) + 1;

    return slice(c_out, { 0, m });
}

template<typename T>
tensor<1, T> irfft(const tensor<1, std::complex<T>>& in) {
    auto n = in.size();
    auto m = 2 * (n - 1);
    auto k = m - n;

    auto slice_in = slice(in, { 1, k + 1 });
    auto rev_in = reverse(slice_in);
    auto conj_in = math::cconj(rev_in);

    auto _in = concatenate(in, conj_in);
    auto out = ifft(_in);

    return math::real(out);
}

template<typename T>
tensor<2, std::complex<T>> fft2(const tensor<2, std::complex<T>>& in) {
    return _fft2_base(in);
}

template<typename T>
tensor<2, std::complex<T>> _fft2_base(const tensor<2, std::complex<T>>& in) {

    #ifdef RACS_MULTITHREADED
        auto out = _fft_parallel_multithreaded(in);
        return _fft_parallel_multithreaded(linalg::transpose(out));
    #else
        auto out = _fft_parallel(in);
        return _fft_parallel(linalg::transpose(out));
    #endif
}

template<typename T>
tensor<2, std::complex<T>> ifft2(const tensor<2, std::complex<T>>& in) {
    return _ifft2_base(in);
}

template<typename T>
tensor<2, std::complex<T>> _ifft2_base(const tensor<2, std::complex<T>>& in) {

    #ifdef RACS_MULTITHREADED
        auto out = _ifft_parallel_multithreaded(in);
        return _ifft_parallel_multithreaded(linalg::transpose(out));
    #else
        auto out = _ifft_parallel(in);
        return _ifft_parallel(linalg::transpose(out));
    #endif
}

template<typename T>
tensor<2, std::complex<T>> _fft_parallel(const tensor<2, std::complex<T>> &in) {
    auto n = in.shape()[0];

    tensor<2, std::complex<T>> out(in.shape());

    for (int i = 0; i < n; ++i) {
        auto _in = slice(in, i);
        auto _out = _fft_base(_in);

        put(out, i, _out);
    }

    return out;
}

template<typename T>
tensor<2, std::complex<T>> _fft_parallel_multithreaded(const tensor<2, std::complex<T>>& in) {
    auto n = in.shape()[0];

    tensor<2, std::complex<T>> out(in.shape());

    runtime::session<tensor<1, std::complex<T>>> session;
    runtime::task_list<tensor<1, std::complex<T>>> tasks;

    for (int i = 0; i < n; ++i) {
        tasks.push_back([&, i] {
            auto _in = slice(in, i);
            auto _out = _fft_base(_in);

            return _out;
        });
    }

    auto callback = session.execute_tasks(std::move(tasks));

    for (size_t i = 0; i < n; ++i) {
        auto _out = callback[i].get();
        put(out, i, _out);
    }

    return out;
}

template<typename T>
tensor<2, std::complex<T>> _ifft_parallel(const tensor<2, std::complex<T>> &in) {
    auto n = in.shape()[0];

    tensor<2, std::complex<T>> out(in.shape());

    for (int i = 0; i < n; ++i) {
        auto _in = slice(in, i);
        auto _out = _ifft_base(_in);

        put(out, i, _out);
    }

    return out;
}

template<typename T>
tensor<2, std::complex<T>> _ifft_parallel_multithreaded(const tensor<2, std::complex<T>>& in) {
    auto n = in.shape()[0];

    tensor<2, std::complex<T>> out(in.shape());

    runtime::session<tensor<1, std::complex<T>>> session;
    runtime::task_list<tensor<1, std::complex<T>>> tasks;

    for (int i = 0; i < n; ++i) {
        tasks.push_back([&, i] {
            auto _in = slice(in, i);
            auto _out = _ifft_base(_in);

            return _out;
        });
    }

    auto callback = session.execute_tasks(std::move(tasks));

    for (size_t i = 0; i < n; ++i) {
        auto _out = callback[i].get();
        put(out, i, _out);
    }

    return out;
}

template<typename T>
tensor<1, std::complex<T>> _radix2(const tensor<1, std::complex<T>>& in,
                                   const tensor<1, std::complex<T>>& wm,
                                   int n) {
    tensor<1, std::complex<T>> out(in.shape());
    simd::radix2(in.data(), out.data(), wm.data(), n);
    return out;
}

template<typename T>
tensor<1, std::complex<T>> _twiddle(size_t n, simd::FFT_TYPE type) {
    tensor<1, std::complex<T>> wm(tensor_shape({ n }));
    simd::twiddle(wm.data(), type, n);
    return wm;
}

template<typename T>
tensor<1, std::complex<T>> _fft_base(const tensor<1, std::complex<T>>& in) {
    auto size = in.size();
    auto log2n = static_cast<size_t>(std::log2(size));
    auto wm = _twiddle<T>(log2n, simd::FFT_TYPE::FORWARD);

    return _radix2(in, wm, size);
}

template<typename T>
tensor<1, std::complex<T>> _ifft_base(const tensor<1, std::complex<T>>& in) {
    auto size = in.size();
    auto log2n = static_cast<size_t>(std::log2(size));
    auto wm = _twiddle<T>(log2n, simd::FFT_TYPE::INVERSE);

    return _radix2(in, wm, size) * (T(1) / size);
}

template<typename T>
tensor<1, std::complex<T>> fftshift(const tensor<1, std::complex<T>>& in) {
    auto n = in.size();

    auto b = slice(in, { 0, n / 2 });
    auto a = slice(in, { n / 2, n });

    return concatenate(a, b);
}

template tensor<1, float> irfft(const tensor<1, std::complex<float>>& in);

template tensor<1, std::complex<float>> fft(const tensor<1, std::complex<float>>& in);
template tensor<1, std::complex<float>> ifft(const tensor<1, std::complex<float>>& in);
template tensor<2, std::complex<float>> fft(const tensor<2, std::complex<float>>& in);
template tensor<2, std::complex<float>> ifft(const tensor<2, std::complex<float>>& in);
template tensor<1, std::complex<float>> rfft(const tensor<1, float>& in);

template tensor<2, std::complex<float>> fft2(const tensor<2, std::complex<float>>& in);
template tensor<2, std::complex<float>> _fft2_base(const tensor<2, std::complex<float>>& in);
template tensor<2, std::complex<float>> _fft_parallel(const tensor<2, std::complex<float>> &in);
template tensor<2, std::complex<float>> _fft_parallel_multithreaded(const tensor<2, std::complex<float>> &in);

template tensor<2, std::complex<float>> ifft2(const tensor<2, std::complex<float>>& in);
template tensor<2, std::complex<float>> _ifft2_base(const tensor<2, std::complex<float>>& in);
template tensor<2, std::complex<float>> _ifft_parallel(const tensor<2, std::complex<float>> &in);
template tensor<2, std::complex<float>> _ifft_parallel_multithreaded(const tensor<2, std::complex<float>> &in);

template tensor<1, std::complex<float>> _twiddle(size_t n, simd::FFT_TYPE type);
template tensor<1, std::complex<float>> _fft_base(const tensor<1, std::complex<float>>& in);
template tensor<1, std::complex<float>> _ifft_base(const tensor<1, std::complex<float>>& in);
template tensor<1, std::complex<float>> fftshift(const tensor<1, std::complex<float>>& in);
template tensor<1, std::complex<float>> _radix2(const tensor<1, std::complex<float>>& in,
                                                const tensor<1, std::complex<float>>& wm,
                                                int n);

}
