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

#include "iir_design.hpp"

namespace racs::signal {

template<typename T>
int rdegree(const zpk<T>& zpk) {
    int p = zpk.p.size();
    int z = zpk.z.size();
    return p - z;
}

template<typename T>
T prewrap(const T wn, T fs) {
    return 2 * fs * std::tan(math::pi<T> * wn / fs);
}

template<typename T>
zpk<T> bilinear(const zpk<T>& zpk, T fs) {
    T fs2 = 2.0 * fs;

    std::cout << fs2 << std::endl;

    auto zd = (fs2 + zpk.z) / (fs2 - zpk.z);
    auto pd = (fs2 + zpk.p) / (fs2 - zpk.p);

    auto y = math::cprod(fs2 - zpk.z);

    auto kd = (zpk.k * math::cprod(fs2 - zpk.z) / math::cprod(fs2 - zpk.p)).real();
    auto zz = pad(zd, 0, rdegree(zpk), std::complex<T>(-1, 0));

    return { zz, pd, kd };
}

template<typename T>
zpk<T> buttap(int n) {
    auto m = arange<T>(-n + 1, n, 2);

    auto p = -math::cexp(std::complex<T>(0, 1) * math::pi<T> * m / (2 * n));

    return { {}, p, 1 };
}

template<typename T>
zpk<T> cheb1ap(int n, T rp) {
    T epsilon = std::sqrt(std::pow(10, (0.1 * rp)) - 1.0);
    T mu = 1.0 / n * std::asinh(1 / epsilon);

    auto m = arange<T>(-n + 1, n, 2);
    auto theta = math::pi<T> * m / (2 * n);
    auto p = -math::csinh(mu + std::complex<T>(0, 1) * theta);

    T k = math::cprod(-p).real();

    if (n % 2 == 0) {
        k = k / std::sqrt(1 + epsilon * epsilon);
    }

    return { {}, p, k };
}

template<typename T>
zpk<T> cheb2ap(int n, T rp) {
    T de = 1.0 / sqrt(pow(10, 0.1 * rp) - 1);
    T mu = asinh(1.0 / de) / n;

    tensor<1, T> m = arange<T>(-n + 1, n, 1);

    if (n % 2 == 1) {
        m = concatenate(arange<T>(-n + 1, 0, 2), arange<T>(2, n, 2));
    } else {
        m = arange<T>(-n + 1, n, 2);
    }

    auto z = -math::cconj(std::complex<T>(0, 1) / math::sin(m * math::pi<T> / (2 * n)));

    auto p0 = -math::cexp(std::complex<T>(0, 1) * math::pi<T> * arange<T>(-n + 1, n, 2) / (2 * n));
    auto p1 = sinhf(mu) * math::real(p0) + std::complex<T>(0, 1) * coshf(mu) * math::imag(p0);

    auto p = 1 / p1;

    auto k = (math::cprod(-p) / math::cprod(-z)).real();

    return {z, p, k};
}

template<typename T>
zpk<T> lp2lp_zpk(const zpk<T>& zpk, T wo) {

    auto z_lp = wo * zpk.z;
    auto p_lp = wo * zpk.p;

    T k_lp = zpk.k * std::pow(wo, rdegree(zpk));

    return { z_lp, p_lp, k_lp };
}

template<typename T>
zpk<T> lp2hp_zpk(const zpk<T>& zpk, T wo) {

    auto z_hp = wo / zpk.z;
    auto p_hp = wo / zpk.p;

    tensor<1, std::complex<T>> z = { -1 };

    if (z_hp.size() > 0) {
        z = z_hp;
    }

    z_hp = pad(z_hp, rdegree(zpk));

    auto k_hp = (zpk.k * math::cprod(-z) / math::cprod(-zpk.p)).real();

    return { z_hp, p_hp, k_hp };
}

template<typename T>
zpk<T> lp2bp_zpk(const zpk<T>& zpk, T wo, T bw) {
    auto degree = rdegree(zpk);

    auto z_lp = zpk.z * bw / 2;
    auto p_lp = zpk.p * bw / 2;

    auto z_bp = concatenate(z_lp + math::csqrt(math::csquare(z_lp) - std::pow(wo, 2)),
                            z_lp - math::csqrt(math::csquare(z_lp) - std::pow(wo, 2)));

    auto p_bp = concatenate(p_lp + math::csqrt(math::csquare(p_lp) - std::pow(wo, 2)),
                            p_lp - math::csqrt(math::csquare(p_lp) - std::pow(wo, 2)));

    z_bp = pad(z_bp, degree);

    T k_bp = zpk.k * std::pow(bw, degree);

    return { z_bp, p_bp, k_bp };
}

template<typename T>
zpk<T> lp2bs_zpk(const zpk<T>& zpk, T wo, T bw) {
    auto degree = rdegree(zpk);

    auto z_hp = (bw / 2) / zpk.z;
    auto p_hp = (bw / 2) / zpk.p;

    auto z_bs = concatenate(z_hp + math::csqrt(math::csquare(z_hp) - std::pow(wo, 2)),
                            z_hp - math::csqrt(math::csquare(z_hp) - std::pow(wo, 2)));

    auto p_bs = concatenate(p_hp + math::csqrt(math::csquare(p_hp) - std::pow(wo, 2)),
                            p_hp - math::csqrt(math::csquare(p_hp) - std::pow(wo, 2)));

    z_bs = pad(z_bs, 0, degree, std::complex<T>(0, 1) * wo);
    p_bs = pad(p_bs, 0, degree, std::complex<T>(0, -1) * wo);

    T k_bs = zpk.k * (math::cprod(-zpk.z) / math::cprod(-zpk.p)).real();

    return { z_bs, p_bs, k_bs };
}

template<typename T>
tensor<2, T> zpk2tf(const zpk<T>& zpk) {
    auto b = math::poly(zpk.z);
    auto a = math::poly(zpk.p);

    auto n = std::max(b.size(), a.size());

    auto b_re = math::real(b);
    auto a_re = math::real(a);

    b_re = b_re * zpk.k;

    tensor<2, T> tf(tensor_shape({ 2, n }));

    b_re = pad(b_re, n - b.size());
    a_re = pad(a_re, n - a.size());

    put(tf, 0, b_re);
    put(tf, 1, a_re);

    return tf;
}

template<typename T>
bool _isreal(std::complex<T> x) {
    return std::abs(x.imag()) < 0.001;
}

template<typename T>
bool _isconj(std::complex<T> a, std::complex<T> b) {
    auto a_re = a.real();
    auto b_re = b.real();
    auto a_im = a.imag();
    auto b_im = b.imag();

    if (a_re != b_re) return false;
    if (std::abs(a_im) - std::abs(b_im) > 0.0001) return false;

    return std::signbit(a_im) != std::signbit(b_im);
}

template<typename T>
tensor<1, T> _zpksos(const zpk<T>& zpk) {
    auto tf = zpk2tf(zpk);

    auto b = slice(tf, 0);
    auto a = slice(tf, 1);

    tensor<1, T> _b = {};
    tensor<1, T> _a = {};

    if (b.size() < 3) {
        _b = pad(b, 0, 3 - b.size());
    } else {
        _b = pad(b, 3 - b.size());
    }

    if (a.size() < 3) {
        _a = pad(a, 0, 3 - a.size());
    } else {
        _a = pad(a, 3 - a.size());
    }

    return { _b(0), _b(1), _b(2), _a(0), _a(1), _a(2) };
}

template<typename T>
size_t _nearest_idx(const tensor<1, std::complex<T>> &fro, std::complex<T> to) {
    auto delta = math::cabs(fro - to);

    T max = 0;
    size_t idx = 0;
    size_t n = delta.size();

    for (size_t i = 0; i < n; ++i) {
        T d = delta(i);

        if (d > max) {
            max = d;
            idx = i;
        }
    }

    return idx;
}

template<typename T>
tensor<2, T> zpk2sos(const zpk<T>& zpk) {
    auto n = (zpk.p.size() + 1) / 2;

    tensor<2, T> sections(tensor_shape({ n, 6 }));

    auto _z = zpk.z;
    auto _p = zpk.p;
    auto k = zpk.k;

    auto i = 0;

    while (_p.size() > 0) {
        std::complex<T> p1 = _p(0);
        _p = del(_p, 0);

        if (_isreal(p1)) {

            if (_z.size() > 0) {
                auto idx = _nearest_idx(_z, p1);
                std::complex<T> z = _z(idx);

                if (_isreal(z)) {
                    auto sos = _zpksos<T>({ {z}, {p1}, 1});
                    put(sections, i++, sos);
                    _z = del(_z, idx);
                } else {
                    auto sos = _zpksos<T>({ {}, {p1}, 1 });
                    put(sections, i++, sos);
                }
            } else {
                auto sos = _zpksos<T>({ {}, {p1}, 1 });
                put(sections, i++, sos);
            }
        } else if (_p.size() > 0) {
            auto idx = _nearest_idx(_p, p1);
            std::complex<T> p2 = _p(idx);
            _p = del(_p, idx);

            idx = _nearest_idx(_z, p1);
            std::complex<T> z1 = _z(idx);
            _z = del(_z, idx);

            idx = _nearest_idx(_z, z1);
            std::complex<T> z2 = _z(idx);
            _z = del(_z, idx);

            auto sos = _zpksos<T>({{z1, z2}, {p1, p2}, 1});
            put(sections, i++, sos);
        }
    }

    auto sos = slice(sections, n - 1);
    put(sections, n - 1 , {sos[0] * zpk.k, sos[1] * zpk.k, sos[2] * zpk.k,
                           sos[3], sos[4], sos[5]});

    return sections;
}

template<typename T>
zpk<T> butter(int n, T wo, BTYPE btype, T fs) {
    zpk<T> zpk = buttap<T>(n);

    return iirfilter<T>(zpk, wo, 0, btype, fs);
}

template<typename T>
zpk<T> butter(int n, T w0, T w1, BTYPE btype, T fs) {
    zpk<T> zpk = buttap<T>(n);

    return iirfilter<T>(zpk, w0, w1, btype, fs);
}

template<typename T>
zpk<T> cheby1(int n, T wo, T rp, BTYPE btype, T fs) {
    zpk<T> zpk = cheb1ap<T>(n, rp);

    return iirfilter<T>(zpk, wo, 0, btype, fs);
}

template<typename T>
zpk<T> cheby1(int n, T w0, T w1, T rp, BTYPE btype, T fs) {
    zpk<T> zpk = cheb1ap<T>(n, rp);

    return iirfilter<T>(zpk, w0, w1, btype, fs);
}

template<typename T>
zpk<T> cheby2(int n, T wo, T rp, BTYPE btype, T fs) {
    zpk<T> zpk = cheb2ap<T>(n, rp);

    return iirfilter<T>(zpk, wo, 0, btype, fs);
}

template<typename T>
zpk<T> cheby2(int n, T w0, T w1, T rp, BTYPE btype, T fs) {
    zpk<T> zpk = cheb2ap<T>(n, rp);

    return iirfilter<T>(zpk, w0, w1, btype, fs);
}

template<typename T>
zpk<T> iirfilter(const zpk<T>& zp, T w0, T w1, BTYPE btype, T fs) {
    zpk<T> _zpk = {};

    w0 = 2 * w0 / fs;
    fs = 2;

    w0 = prewrap(w0, fs);
    T bw = 0;

    if (btype == BTYPE::BANDPASS || btype == BTYPE::BANDSTOP) {
        bw = w1 - w0;
        w0 = sqrt(w0 * w1);
    }

    switch (btype) {
        case BTYPE::LOWPASS:
            _zpk = lp2lp_zpk(zp, w0);
            break;
        case BTYPE::HIGHPASS:
            _zpk = lp2hp_zpk(zp, w0);
            break;
        case BTYPE::BANDPASS:
            _zpk = lp2bp_zpk(zp, w0, bw);
            break;
        case BTYPE::BANDSTOP:
            _zpk = lp2bs_zpk(zp, w0, bw);
            break;
    }

    return bilinear(_zpk, fs);
}

template<typename T>
tensor<1, T> sosfilt(const tensor<2, T>& sos, const tensor<1, T>& x) {
    tensor<1, T> out = x;
    auto n = sos.shape()[0];

    RACS_UNROLL(4)
    for (int i = 0; i < n; ++i) {
        auto section = slice(sos, i);
        out = biquad(out, section);
    }

    return out;
}

template<typename T>
tensor<2, T> sosfilt(const tensor<2, T>& sos, const tensor<2, T>& x) {
    #ifdef RACS_MULTITHREADED
        return _sosfilt_parallel_multithreaded(sos, x);
    #else
        return _sosfilt_parallel(sos, x);
    #endif
}

template<typename T>
tensor<2, T> _sosfilt_parallel(const tensor<2, T>& sos, const tensor<2, T>& x) {
    auto num_channels = x.shape()[0];
    auto num_samples = x.shape()[1];

    tensor<2, T> out(tensor_shape({ num_channels, num_samples }));

    for (size_t channel = 0; channel < num_channels; ++channel) {
        auto in = slice(x, channel);
        auto _out = sosfilt(sos, in);
        put(out, channel, _out);
    }

    return out;
}

template<typename T>
tensor<2, T> _sosfilt_parallel_multithreaded(const tensor<2, T>& sos, const tensor<2, T>& x) {
    runtime::session<tensor<1, T>> session;
    runtime::task_list<tensor<1, T>> tasks;

    auto num_channels = x.shape()[0];

    for (size_t channel = 0; channel < num_channels; ++channel) {
        tasks.push_back([&, channel] {
            auto in = slice(x, channel);
            auto out = sosfilt(sos, in);
            return out;
        });
    }

    auto callback = session.execute_tasks(std::move(tasks));
    tensor<2, T> out(x.shape());

    for (size_t channels = 0; channels < num_channels; ++channels) {
        put(out, channels, callback[channels].get());
    }

    return out;
}

template int rdegree(const zpk<float>& zpk);

template float prewrap(float wn, float fs);

template zpk<float> bilinear(const zpk<float>& zpk, float fs);

template zpk<float> buttap(int n);

template zpk<float> cheb1ap(int n, float rp);

template zpk<float> cheb2ap(int n, float rp);

template zpk<float> lp2lp_zpk(const zpk<float>& zpk, float wo);

template zpk<float> lp2hp_zpk(const zpk<float>& zpk, float wo);

template zpk<float> lp2bp_zpk(const zpk<float>& zpk, float wo, float bw);

template zpk<float> lp2bs_zpk(const zpk<float>& zpk, float wo, float bw);

template tensor<2, float> zpk2tf(const zpk<float>& zpk);

template bool _isreal(const std::complex<float> x);

template size_t _nearest_idx(const tensor<1, std::complex<float>> &fro, std::complex<float> to);

template tensor<2, float> zpk2sos(const zpk<float>& zpk);

template tensor<1, float> _zpksos(const zpk<float>& zpk);

template zpk<float> butter(int n, float wo, BTYPE ftype, float fs);

template zpk<float> butter(int n, float w0, float w1, BTYPE ftype, float fs);

template zpk<float> cheby1(int n, float wo, float rp, BTYPE ftype, float fs);

template zpk<float> cheby1(int n, float w0, float w1, float rp, BTYPE ftype, float fs);

template zpk<float> cheby2(int n, float wo, float rp, BTYPE ftype, float fs);

template zpk<float> cheby2(int n, float w0, float w1, float rp, BTYPE ftype, float fs);

template zpk<float> iirfilter(const zpk<float>& zpk, float w0, float w1, BTYPE ftype, float fs);

template tensor<1, float> sosfilt(const tensor<2, float>& sos, const tensor<1, float>& x);

template tensor<2, float> sosfilt(const tensor<2, float>& sos, const tensor<2, float>& x);

template tensor<2, float> _sosfilt_parallel(const tensor<2, float>& sos,
                                            const tensor<2, float>& x);

template tensor<2, float> _sosfilt_parallel_multithreaded(const tensor<2, float>& sos,
                                                          const tensor<2, float>& x);

}