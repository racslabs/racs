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

#include "complex_neon_f32.hpp"

namespace racs::simd {

#ifdef __ARM_NEON__

void complex(float* x, std::complex<float>* y, size_t n) {
    size_t i = 0;

    if (n >= 2) {
        for ( ; i < n; i += 2) {
            v128 _r = vld1q_f32(x + i);
            v128 _c = vr2cq_f32(_r);
            vst1q_f32(reinterpret_cast<float *>(y) + i * 2, _c);
        }
    }

    for ( ; i < n; ++i) {
        y[i] = x[i];
    }
}

void cabs(std::complex<float>* x, float* y, size_t n) {
    size_t i = 0;

    if (n >= 2) {
        for (; i < n; i += 2) {
            v128 _x = vld1q_f32(reinterpret_cast<float *>(x + i));
            v64 _y = vmagq_f32(_x);
            vst1_f32(y + i, _y);
        }
    }

    for ( ; i < n; ++i) {
        y[i] = std::abs(x[i]);
    }
}

void real(std::complex<float>* x, float* y, size_t n) {
    size_t i = 0;

    if (n >= 2) {
        for ( ; i < n; i += 2) {
            v128 _x = vld1q_f32(reinterpret_cast<float *>(x + i));
            v64 _re = vrealq_f32(_x);
            vst1_f32(y + i, _re);
        }
    }

    for ( ; i < n; ++i) {
        y[i] = x[i].real();
    }
}

void imag(std::complex<float>* x, float* y, size_t n) {
    size_t i = 0;

    if (n >= 2) {
        for ( ; i < n; i += 2) {
            v128 _x = vld1q_f32(reinterpret_cast<float *>(x + i));
            v64 _re = vimagq_f32(_x);
            vst1_f32(y + i, _re);
        }
    }

    for ( ; i < n; ++i) {
        y[i] = x[i].imag();
    }
}

void ccumprod(const std::complex<float> *a, std::complex<float> *b, size_t n) {
    size_t i = 1;

    //TODO: add neon

    b[0] = a[0];
    for ( ; i < n; ++i) {
        b[i] = a[i] * b[i - 1];
    }
}

void cexp(std::complex<float>* a, std::complex<float>* b, size_t n) {
    size_t i = 0;

    if (n >= 2) {
        for ( ; i < n; i += 2) {
            v128 _a = vld1q_f32(reinterpret_cast<float*>(a + i));
            v128 _b = vcexpzq_f32(_a);
            vst1q_f32(reinterpret_cast<float*>(b + i), _b);
        }
    }

    for ( ; i < n; ++i) {
        b[i] = std::exp(a[i]);
    }
}

void csinh(std::complex<float>* a, std::complex<float>* b, size_t n) {
    size_t i = 0;

    if (n >= 2) {
        for ( ; i < n; i += 2) {
            v128 _a = vld1q_f32(reinterpret_cast<float*>(a + i));
            v128 _b = vcsinhq_f32(_a);
            vst1q_f32(reinterpret_cast<float*>(b + i), _b);
        }
    }

    for ( ; i < n; ++i) {
        b[i] = std::sinh(a[i]);
    }
}

void cconj(std::complex<float>* a, std::complex<float>* b, size_t n) {
    size_t i = 0;

    if (n >= 2) {
        for ( ; i < n; i += 2) {
            v128 _a = vld1q_f32(reinterpret_cast<float*>(a + i));
            v128 _b = vcconjq_f32(_a);
            vst1q_f32(reinterpret_cast<float*>(b + i), _b);
        }
    }

    for ( ; i < n; ++i) {
        b[i] = std::complex<float>(a[i].real(), -a[i].imag());
    }
}

void csquare(std::complex<float>* a, std::complex<float>* b, size_t n) {
    size_t i = 0;

    if (n >= 2) {
        for ( ; i < n; i += 2) {
            v128 _a = vld1q_f32(reinterpret_cast<float*>(a + i));
            v128 _b = vcsquareq_f32(_a);
            vst1q_f32(reinterpret_cast<float*>(b + i), _b);
        }
    }

    for ( ; i < n; ++i) {
        b[i] = a[i] * a[i];
    }
}

void csqrt(std::complex<float>* a, std::complex<float>* b, size_t n) {
    size_t i = 0;

    //TODO: add neon
    for ( ; i < n; ++i) {
        b[i] = std::sqrt(a[i]);
    }
}
#endif

}
