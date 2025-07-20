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

#ifndef RACS_OP_HPP
#define RACS_OP_HPP

#include "tensor.hpp"
#include "bitcast.hpp"
#include "../math/complex.hpp"
#include "../runtime/exceptions.hpp"

namespace racs {

#define RACS_CHECK_EQUAL_SHAPE(x, y) \
    std::stringstream ss;             \
    if (x.shape()[0] != y.shape()[0]) { \
        ss << x.shape() << " does not equal " << y.shape(); \
        throw runtime::zf100_exception(ss.str());            \
    }

template<typename T, typename U>
std::complex<U> complex_cast(const std::complex<T>& c) {
    return { static_cast<U>(c.real()), static_cast<U>(c.imag()) };
}

template<int NDIMS, typename T1, typename T2>
tensor<NDIMS, std::complex<T2>> operator+(std::complex<T1> x, const tensor<NDIMS, std::complex<T2>> &y) {
    tensor<NDIMS, std::complex<T2>> out(y.shape());
    out.fill(complex_cast<T1, T2>(x));

    auto _y = bitcast<T2>(y);
    auto _out = bitcast<T2>(out);
    simd::add(_out.data(), _y.data(), _out.data(), _y.size());

    return out;
}

template<int NDIMS, typename T1, typename T2>
tensor<NDIMS, std::complex<T2>> operator+(T1 x, const tensor<NDIMS, std::complex<T2>> &y) {
    tensor<NDIMS, std::complex<T2>> out(y.shape());
    out.fill(x);

    auto _y = bitcast<T2>(y);
    auto _out = bitcast<T2>(out);
    simd::add(_out.data(), _y.data(), _out.data(), _y.size());

    return out;
}

template<int NDIMS, typename T1, typename T2>
tensor<NDIMS, std::complex<T2>> operator+(const tensor<NDIMS, T1>& x, const tensor<NDIMS, std::complex<T2>>& y) {
    tensor<NDIMS, std::complex<T2>> out(y.shape());

    auto c_x = math::complex(x);

    auto _x = bitcast<T2>(c_x);
    auto _y = bitcast<T2>(y);
    auto _out = bitcast<T2>(out);
    simd::add(_x.data(), _y.data(), _out.data(), _y.size());

    return out;
}


template<int NDIMS, typename T1, typename T2>
tensor<NDIMS, std::complex<T2>> operator+(std::complex<T1> x, const tensor<NDIMS, T2> &y) {
    tensor<NDIMS, std::complex<T2>> out(y.shape());
    out.fill(complex_cast<T1, T2>(x));

    auto c_y = math::complex(y);
    auto _y = bitcast<T2>(c_y);
    auto _out = bitcast<T2>(out);

    simd::add(_out.data(), _y.data(), _out.data(), _y.size());

    return out;
}

template<int NDIMS, typename T1, typename T2>
tensor<NDIMS, T2> operator+(T1 x, const tensor<NDIMS, T2> &y) {
    tensor<NDIMS, T2> out(y.shape());
    out.fill(x);

    simd::add(out.data(), y.data(), out.data(), y.size());

    return out;
}

template<int NDIMS, typename T1, typename T2>
tensor<NDIMS, std::complex<T2>> operator-(std::complex<T1> x, const tensor<NDIMS, std::complex<T2>> &y) {
    tensor<NDIMS, std::complex<T2>> out(y.shape());
    out.fill(complex_cast<T1, T2>(x));

    auto _y = bitcast<T2>(y);
    auto _out = bitcast<T2>(out);
    simd::sub(_out.data(), _y.data(), _out.data(), _y.size());

    return out;
}

template<int NDIMS, typename T>
tensor<NDIMS, std::complex<T>> operator-(const tensor<NDIMS, std::complex<T>> &x) {
    tensor<NDIMS, std::complex<T>> out(x.shape());
    out.fill(-1);

    simd::cmplxmul(out.data(), x.data(), out.data(), x.size());

    return out;
}

template<int NDIMS, typename T1, typename T2>
tensor<NDIMS, std::complex<T2>> operator-(T1 x, const tensor<NDIMS, std::complex<T2>> &y) {
    tensor<NDIMS, std::complex<T2>> out(y.shape());
    out.fill(x);

    auto _y = bitcast<T2>(y);
    auto _out = bitcast<T2>(out);
    simd::sub(_out.data(), _y.data(), _out.data(), _y.size());

    return out;
}

template<int NDIMS, typename T1, typename T2>
tensor<NDIMS, std::complex<T2>> operator-(std::complex<T1> x, const tensor<NDIMS, T2> &y) {
    tensor<NDIMS, std::complex<T2>> out(y.shape());
    out.fill(complex_cast<T1, T2>(x));

    auto c_y = math::complex(y);
    auto _y = bitcast<T2>(c_y);
    auto _out = bitcast<T2>(out);

    simd::sub(_out.data(), _y.data(), _out.data(), _y.size());

    return out;
}

template<int NDIMS, typename T1, typename T2>
tensor<NDIMS, T2> operator-(T1 x, const tensor<NDIMS, T2> &y) {
    tensor<NDIMS, T2> out(y.shape());
    out.fill(x);

    simd::sub(out.data(), y.data(), out.data(), y.size());

    return out;
}

template<int NDIMS, typename T1, typename T2>
tensor<NDIMS, std::complex<T2>> operator*(std::complex<T1> x, const tensor<NDIMS, std::complex<T2>> &y) {
    tensor<NDIMS, std::complex<T2>> out(y.shape());
    out.fill(complex_cast<T1, T2>(x));

    simd::cmplxmul(out.data(), y.data(), out.data(), y.size());

    return out;
}

template<int NDIMS, typename T1, typename T2>
tensor<NDIMS, std::complex<T2>> operator*(T1 x, const tensor<NDIMS, std::complex<T2>> &y) {
    tensor<NDIMS, std::complex<T2>> out(y.shape());
    out.fill(x);

    simd::cmplxmul(out.data(), y.data(), out.data(), y.size());

    return out;
}

template<int NDIMS, typename T1, typename T2>
tensor<NDIMS, std::complex<T1>> operator*(std::complex<T1> x, const tensor<NDIMS, T2> &y) {
    tensor<NDIMS, std::complex<T1>> out(y.shape());
    out.fill(complex_cast<T2, T1>(x));

    auto c_y = math::complex(y);
    simd::cmplxmul(out.data(), c_y.data(), out.data(), y.size());

    return out;
}

template<int NDIMS, typename T1, typename T2>
tensor<NDIMS, T2> operator*(T1 x, const tensor<NDIMS, T2> &y) {
    tensor<NDIMS, T2> out(y.shape());
    out.fill(static_cast<float>(x));

    simd::mul(out.data(), y.data(), out.data(), y.size());

    return out;
}

template<int NDIMS, typename T1, typename T2>
tensor<NDIMS, std::complex<T2>> operator/(std::complex<T1> x, const tensor<NDIMS, std::complex<T2>> &y) {
    tensor<NDIMS, std::complex<T2>> out(y.shape());
    out.fill(complex_cast<T1, T2>(x));

    simd::cmplxdiv(out.data(), y.data(), out.data(), y.size());

    return out;
}

template<int NDIMS, typename T1, typename T2>
tensor<NDIMS, std::complex<T2>> operator/(T1 x, const tensor<NDIMS, std::complex<T2>> &y) {
    tensor<NDIMS, std::complex<T2>> out(y.shape());
    out.fill(x);

    simd::cmplxdiv(out.data(), y.data(), out.data(), y.size());

    return out;
}

template<int NDIMS, typename T1, typename T2>
tensor<NDIMS, std::complex<T2>> operator/(std::complex<T1> x, const tensor<NDIMS, T2> &y) {
    tensor<NDIMS, std::complex<T2>> out(y.shape());
    out.fill(complex_cast<T1, T2>(x));

    auto y_c = math::complex(y);
    simd::cmplxdiv(out.data(), y_c.data(), out.data(), y.size());

    return out;
}

template<int NDIMS, typename T1, typename T2>
tensor<NDIMS, T2> operator/(T1 x, const tensor<NDIMS, T2> &y) {
    tensor<NDIMS, T2> out(y.shape());
    out.fill(x);

    simd::div(out.data(), y.data(), out.data(), y.size());

    return out;
}

template<int NDIMS, typename T1, typename T2>
tensor<NDIMS, std::complex<T1>> operator+(const tensor<NDIMS, std::complex<T1>> &x, T2 y) {
    tensor<NDIMS, std::complex<T1>> out(x.shape());
    out.fill(y);

    auto _x = bitcast<T1>(x);
    auto _out = bitcast<T1>(out);
    simd::add(_x.data(), _out.data(), _out.data(), _x.size());

    return out;
}

template<int NDIMS, typename T1, typename T2>
tensor<NDIMS, std::complex<T1>> operator+(const tensor<NDIMS, T1> &x, std::complex<T2> y) {
    tensor<NDIMS, std::complex<T1>> out(x.shape());
    out.fill(complex_cast<T2, T1>(y));

    auto c_x = math::complex(x);
    auto _x = bitcast<T1>(c_x);
    auto _out = bitcast<T1>(out);

    simd::add(_x.data(), _out.data(), _out.data(), _x.size());

    return out;
}

template<int NDIMS, typename T1, typename T2>
tensor<NDIMS, T1> operator+(const tensor<NDIMS, T1> &x, T2 y) {
    tensor<NDIMS, T1> out(x.shape());
    out.fill(y);

    simd::add(x.data(), out.data(), out.data(), x.size());

    return out;
}

template<int NDIMS, typename T1, typename T2>
tensor<NDIMS, std::complex<T1>> operator-(const tensor<NDIMS, std::complex<T1>> &x, T2 y) {
    tensor<NDIMS, std::complex<T1>> out(x.shape());
    out.fill(y);

    auto _x = bitcast<T1>(x);
    auto _out = bitcast<T1>(out);
    simd::sub(_x.data(), _out.data(), _out.data(), _x.size());

    return out;
}

template<int NDIMS, typename T1, typename T2>
tensor<NDIMS, std::complex<T1>> operator-(const tensor<NDIMS, T1> &x, std::complex<T2> y) {
    tensor<NDIMS, std::complex<T1>> out(x.shape());
    out.fill(complex_cast<T2, T1>(y));

    auto c_x = math::complex(x);
    auto _x = bitcast<T1>(c_x);
    auto _out = bitcast<T1>(out);

    simd::sub(_x.data(), _out.data(), _out.data(), _x.size());

    return out;
}

template<int NDIMS, typename T1, typename T2>
tensor<NDIMS, std::complex<T1>> operator-(const tensor<NDIMS, std::complex<T1>> &x, std::complex<T2> y) {
    tensor<NDIMS, std::complex<T1>> out(x.shape());
    out.fill(complex_cast<T2, T1>(y));

    auto _x = bitcast<T1>(x);
    auto _out = bitcast<T1>(out);

    simd::sub(_x.data(), _out.data(), _out.data(), _x.size());

    return out;
}

template<int NDIMS, typename T1, typename T2>
tensor<NDIMS, T1> operator-(const tensor<NDIMS, T1> &x, T2 y) {
    tensor<NDIMS, T1> out(x.shape());
    out.fill(y);

    simd::sub(x.data(), out.data(), out.data(), x.size());

    return out;
}

template<int NDIMS, typename T1, typename T2>
tensor<NDIMS, std::complex<T1>> operator*(const tensor<NDIMS, std::complex<T1>> &x, T2 y) {
    tensor<NDIMS, std::complex<T1>> out(x.shape());
    out.fill(y);

    simd::cmplxmul(x.data(), out.data(), out.data(), x.size());

    return out;
}

template<int NDIMS, typename T1, typename T2>
tensor<NDIMS, std::complex<T1>> operator*(const tensor<NDIMS, T1> &x, std::complex<T2> y) {
    tensor<NDIMS, std::complex<T1>> out(x.shape());
    out.fill(complex_cast<T2, T1>(y));

    auto c_x = math::complex(x);
    simd::cmplxmul(c_x.data(), out.data(), out.data(), x.size());

    return out;
}

template<int NDIMS, typename T1, typename T2>
tensor<NDIMS, T1> operator*(const tensor<NDIMS, T1> &x, T2 y) {
    tensor<NDIMS, T1> out(x.shape());
    out.fill(y);

    simd::mul(x.data(), out.data(), out.data(), x.size());

    return out;
}

template<int NDIMS, typename T1, typename T2>
tensor<NDIMS, std::complex<T1>> operator/(const tensor<NDIMS, std::complex<T1>> &x, T2 y) {
    tensor<NDIMS, std::complex<T1>> out(x.shape());
    out.fill(y);

    simd::cmplxdiv(x.data(), out.data(), out.data(), x.size());

    return out;
}

template<int NDIMS, typename T1, typename T2>
tensor<NDIMS, std::complex<T1>> operator/(const tensor<NDIMS, T1> &x, std::complex<T2> y) {
    tensor<NDIMS, std::complex<T1>> out(x.shape());
    out.fill(complex_cast<T2, T1>(y));

    auto c_x = math::complex(x);
    simd::cmplxdiv(c_x.data(), out.data(), out.data(), x.size());

    return out;
}

template<int NDIMS, typename T1, typename T2>
tensor<NDIMS, T1> operator/(const tensor<NDIMS, T1> &x, T2 y) {
    tensor<NDIMS, T1> out(x.shape());
    out.fill(y);

    simd::div(x.data(), out.data(), out.data(), x.size());

    return out;
}

template<int NDIMS, typename T>
tensor<NDIMS, std::complex<T>> operator+(const tensor<NDIMS, std::complex<T>> &x,
                                         const tensor<NDIMS, std::complex<T>> &y) {
    tensor<NDIMS, std::complex<T>> out(x.shape());

    auto _x = bitcast<T>(x);
    auto _y = bitcast<T>(y);
    auto _out = bitcast<T>(out);
    simd::add(_x.data(), _y.data(), _out.data(), _y.size());

    return out;
}

template<int NDIMS, typename T>
tensor<NDIMS, T> operator+(const tensor<NDIMS, T> &x, const tensor<NDIMS, T> &y) {
    tensor<NDIMS, T> out(x.shape());
    simd::add(x.data(), y.data(), out.data(), y.size());

    return out;
}

template<int NDIMS, typename T>
tensor<NDIMS, std::complex<T>> operator+=(const tensor<NDIMS, std::complex<T>> &x,
                                          const tensor<NDIMS, std::complex<T>> &y) {
    auto _x = bitcast<T>(x);
    auto _y = bitcast<T>(y);
    simd::add(_x.data(), _y.data(), _x.data(), _y.size());

    return x;
}

template<int NDIMS, typename T>
tensor<NDIMS, T>& operator+=(tensor<NDIMS, T> &x, const tensor<NDIMS, T> &y) {
    simd::add(x.data(), y.data(), x.data(), y.size());

    return x;
}

template<int NDIMS, typename T>
tensor<NDIMS, std::complex<T>> operator-=(const tensor<NDIMS, std::complex<T>> &x,
                                          const tensor<NDIMS, std::complex<T>> &y) {
    auto _x = bitcast<T>(x);
    auto _y = bitcast<T>(y);
    simd::sub(_x.data(), _y.data(), _x.data(), _y.size());

    return x;
}

template<int NDIMS, typename T>
tensor<NDIMS, T>& operator-=(tensor<NDIMS, T> &x, const tensor<NDIMS, T> &y) {
    simd::sub(x.data(), y.data(), x.data(), y.size());

    return x;
}

template<int NDIMS, typename T>
tensor<NDIMS, std::complex<T>> operator-(const tensor<NDIMS, std::complex<T>> &x,
                                         const tensor<NDIMS, std::complex<T>> &y) {
    tensor<NDIMS, std::complex<T>> out(x.shape());

    auto _x = bitcast<T>(x);
    auto _y = bitcast<T>(y);
    auto _out = bitcast<T>(out);
    simd::sub(_x.data(), _y.data(), _out.data(), _y.size());

    return out;
}

template<int NDIMS, typename T>
tensor<NDIMS, T> operator-(const tensor<NDIMS, T> &x, const tensor<NDIMS, T> &y) {
    tensor<NDIMS, T> out(x.shape());

    simd::sub(x.data(), y.data(), out.data(), y.size());

    return out;
}

template<int NDIMS, typename T>
tensor<NDIMS, std::complex<T>> operator*(const tensor<NDIMS, std::complex<T>> &x,
                                         const tensor<NDIMS, std::complex<T>> &y) {
    tensor<NDIMS, std::complex<T>> out(x.shape());
    simd::cmplxmul(x.data(), y.data(), out.data(), y.size());
    return out;
}

template<int NDIMS, typename T>
tensor<NDIMS, T> operator*(const tensor<NDIMS, T> &x, const tensor<NDIMS, T> &y) {
    tensor<NDIMS, T> out(x.shape());

    simd::mul(x.data(), y.data(), out.data(), y.size());

    return out;
}

template<int NDIMS, typename T>
tensor<NDIMS, std::complex<T>> operator/(const tensor<NDIMS, std::complex<T>> &x,
                                         const tensor<NDIMS, std::complex<T>> &y) {
    tensor<NDIMS, std::complex<T>> out(x.shape());
    simd::cmplxdiv(x.data(), y.data(), out.data(), y.size());

    return out;
}

template<int NDIMS, typename T>
tensor<NDIMS, T> operator/(const tensor<NDIMS, T> &x, const tensor<NDIMS, T> &y) {
    tensor<NDIMS, T> out(x.shape());
    simd::div(x.data(), y.data(), out.data(), y.size());

    return out;
}

template<int NDIMS, typename T>
tensor<NDIMS, T> operator%(const tensor<NDIMS, T> &x, const tensor<NDIMS, T> &y) {
    tensor<NDIMS, T> out(x.shape());
    simd::mod(x.data(), y.data(), out.data(), y.size());

    return out;
}

template<int NDIMS, typename T1, typename T2>
tensor<NDIMS, T1> operator%(const tensor<NDIMS, T1> &x, T2 y) {
    tensor<NDIMS, T1> out(x.shape());
    out.fill(y);

    simd::mod(x.data(), out.data(), out.data(), x.size());
    return out;
}

}

#endif //RACS_OP_HPP