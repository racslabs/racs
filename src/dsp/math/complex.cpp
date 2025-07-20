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

#include "complex.hpp"

namespace racs::math {

template<typename T>
tensor<1, T> real(const tensor<1, std::complex<T>>& in) {
    if (in.shape().ndims() != 0) {
        tensor<1, T> out(in.shape());
        simd::real(in.data(), out.data(), in.size());
        return out;
    }

    return {};
}

template<typename T>
tensor<1, T> imag(const tensor<1, std::complex<T>>& in) {
    if (in.shape().ndims() != 0) {
        tensor<1, T> out(in.shape());
        simd::imag(in.data(), out.data(), in.size());
        return out;
    }

    return {};
}

template<int NDIMS, typename T>
tensor<NDIMS, std::complex<T>> complex(const tensor<NDIMS, T>& in) {
    if (in.shape().ndims() != 0) {
        tensor<NDIMS, std::complex<T>> out(in.shape());
        simd::complex(in.data(), out.data(), in.size());
        return out;
    }

    return {};
}

template<int NDIMS, typename T>
tensor<NDIMS, std::complex<T>> cexp(const tensor<NDIMS, std::complex<T>> &x) {
    if (x.shape().ndims() != 0) {
        tensor<NDIMS, std::complex<T>> out(x.shape());
        simd::cexp(x.data(), out.data(), x.size());
        return out;
    }

    return {};
}

template<int NDIMS, typename T>
tensor<NDIMS, std::complex<T>> csinh(const tensor<NDIMS, std::complex<T>>& x) {
    if (x.shape().ndims() != 0) {
        tensor<NDIMS, std::complex<T>> out(x.shape());
        simd::csinh(x.data(), out.data(), x.size());
        return out;
    }

    return {};
}

template<typename T>
tensor<1, std::complex<T>> ccumprod(const tensor<1, std::complex<T>>& x) {
    if (x.shape().ndims() != 0) {
        tensor<1, std::complex<T>> out(x.shape());
        simd::ccumprod(x.data(), out.data(), x.size());
        return out;
    }

    return { 1 };
}

template<typename T>
std::complex<T> cprod(const tensor<1, std::complex<T>>& x) {
    if (x.shape().ndims() != 0) {
        return ccumprod(x)(x.size() - 1);
    }

    return 1;
}

template<int NDIMS, typename T>
tensor<NDIMS, T> cabs(const tensor<NDIMS, std::complex<T>>& in) {
    if (in.shape().ndims() != 0) {
        tensor<NDIMS, T> out(in.shape());
        simd::cabs(in.data(), out.data(), in.size());
        return out;
    }

    return {};
}

template<int NDIMS, typename T>
tensor<NDIMS, std::complex<T>> cconj(const tensor<NDIMS, std::complex<T>>& x) {
    if (x.shape().ndims() != 0) {
        tensor<NDIMS, std::complex<T>> out(x.shape());
        simd::cconj(x.data(), out.data(), x.size());
        return out;
    }

    return {};
}

template<int NDIMS, typename T>
tensor<NDIMS, std::complex<T>> csquare(const tensor<NDIMS, std::complex<T>>& x) {
    if (x.shape().ndims() != 0) {
        tensor<NDIMS, std::complex<T>> out(x.shape());
        simd::csquare(x.data(), out.data(), x.size());
        return out;
    }

    return {};
}

template<int NDIMS, typename T>
tensor<NDIMS, std::complex<T>> csqrt(const tensor<NDIMS, std::complex<T>>& x) {
    if (x.shape().ndims() != 0) {
        tensor<NDIMS, std::complex<T>> out(x.shape());
        simd::csqrt(x.data(), out.data(), x.size());
        return out;
    }

    return {};
}

template tensor<1, float> real(const tensor<1, std::complex<float>>& in);

template tensor<1, float> imag(const tensor<1, std::complex<float>>& in);

template tensor<1, std::complex<float>> complex(const tensor<1, float> &in);
template tensor<2, std::complex<float>> complex(const tensor<2, float> &in);
template tensor<3, std::complex<float>> complex(const tensor<3, float> &in);

template tensor<1, std::complex<float>> cexp(const tensor<1, std::complex<float>> &x);
template tensor<2, std::complex<float>> cexp(const tensor<2, std::complex<float>> &x);
template tensor<3, std::complex<float>> cexp(const tensor<3, std::complex<float>> &x);

template tensor<1, std::complex<float>> csinh(const tensor<1, std::complex<float>> &x);
template tensor<2, std::complex<float>> csinh(const tensor<2, std::complex<float>> &x);
template tensor<3, std::complex<float>> csinh(const tensor<3, std::complex<float>> &x);

template std::complex<float> cprod(const tensor<1, std::complex<float>> &x);

template tensor<1, std::complex<float>> ccumprod(const tensor<1, std::complex<float>>& x);

template tensor<1, float> cabs(const tensor<1, std::complex<float>>& in);
template tensor<2, float> cabs(const tensor<2, std::complex<float>>& in);
template tensor<3, float> cabs(const tensor<3, std::complex<float>>& in);

template tensor<1, std::complex<float>> cconj(const tensor<1, std::complex<float>>& x);
template tensor<2, std::complex<float>> cconj(const tensor<2, std::complex<float>>& x);
template tensor<3, std::complex<float>> cconj(const tensor<3, std::complex<float>>& x);

template tensor<1, std::complex<float>> csquare(const tensor<1, std::complex<float>>& x);
template tensor<2, std::complex<float>> csquare(const tensor<2, std::complex<float>>& x);
template tensor<3, std::complex<float>> csquare(const tensor<3, std::complex<float>>& x);

template tensor<1, std::complex<float>> csqrt(const tensor<1, std::complex<float>>& x);
template tensor<2, std::complex<float>> csqrt(const tensor<2, std::complex<float>>& x);
template tensor<3, std::complex<float>> csqrt(const tensor<3, std::complex<float>>& x);

}
