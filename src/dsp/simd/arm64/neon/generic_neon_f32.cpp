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

#include "generic_neon_f32.hpp"

namespace racs::simd {

#ifdef __ARM_NEON__

void add(float *a, float *b, float *c, size_t n) {
    size_t i = 0;

    if (n >= 4) {
        for (; i < n; i += 4) {
            v128 _a = vld1q_f32(a + i);
            v128 _b = vld1q_f32(b + i);
            v128 _c = vaddq_f32(_a, _b);

            vst1q_f32(c + i, _c);
        }
    }

    for ( ; i < n; ++i) {
        c[i] = a[i] + b[i];
    }
}

void sub(float *a, float *b, float *c, size_t n) {
    size_t i = 0;

    if (n >= 4) {
        for (; i < n; i += 4) {
            v128 _a = vld1q_f32(a + i);
            v128 _b = vld1q_f32(b + i);
            v128 _c = vsubq_f32(_a, _b);

            vst1q_f32(c + i, _c);
        }
    }

    for ( ; i < n; ++i) {
        c[i] = a[i] - b[i];
    }
}

void mul(float *a, float *b, float *c, size_t n) {
    size_t i = 0;

    if (n >= 4) {
        for (; i < n; i += 4) {
            v128 _a = vld1q_f32(a + i);
            v128 _b = vld1q_f32(b + i);
            v128 _c = vmulq_f32(_a, _b);

            vst1q_f32(c + i, _c);
        }
    }

    for ( ; i < n; ++i) {
        c[i] = a[i] * b[i];
    }
}

void div(float *a, float *b, float *c, size_t n) {
    size_t i = 0;

    if (n >= 4) {
        for (; i < n; i += 4) {
            v128 _a = vld1q_f32(a + i);
            v128 _b = vld1q_f32(b + i);
            v128 _c = vdivq_f32(_a, _b);

            vst1q_f32(c + i, _c);
        }
    }

    for ( ; i < n; ++i) {
        c[i] = a[i] / b[i];
    }
}

void mod(float *a, float *b, float *c, size_t n) {
    size_t i = 0;

    if (n >= 4) {
        for (; i < n; i += 4) {
            v128 _a = vld1q_f32(a + i);
            v128 _b = vld1q_f32(b + i);
            v128 _c = vmodq_f32(_a, _b);

            vst1q_f32(c + i, _c);
        }
    }

    for ( ; i < n; ++i) {
        c[i] = mod(a[i], b[i]);
    }
}

void rev(float* a, float* b, size_t n) {
    size_t i = 0;

    if (n >= 4) {
        for (; i < n; i += 4) {
            v128 _a = vld1q_f32(a + i);
            v128 _b = vrevq_f32(_a);
            vst1q_f32(b + n - i - 4, _b);
        }
    }

    for ( ; i < n; ++i) {
        b[n - i - 1] = a[i];
    }
}

void crev(std::complex<float>* a, std::complex<float>* b, size_t n) {
    size_t i = 0;
    size_t k = n % 2;

    if (n >= 2) {
        for ( ; i < n - k; i += 2) {
            v128 _a = vld1q_f32(reinterpret_cast<float*>(a + i));
            v128 _b = vcrevq_f32(_a);
            vst1q_f32(reinterpret_cast<float*>(b + n - i - 2), _b);
        }
    }

    for ( ; i < n; ++i) {
        b[n - i - 1] = a[i];
    }
}

void cmplxmul(std::complex<float>* a, std::complex<float>* b, std::complex<float>* c, size_t n) {
    size_t i = 0;

    if (n >= 2) {
        for ( ; i < n; i += 2) {
            v128 _a = vld1q_f32(reinterpret_cast<float *>(a + i));
            v128 _b = vld1q_f32(reinterpret_cast<float *>(b + i));
            v128 _c = vcmulq_f32(_a, _b);

            vst1q_f32(reinterpret_cast<float *>(c + i), _c);
        }
    }

    for ( ; i < n; ++i) {
        c[i] = a[i] * b[i];
    }
}

void cmplxdiv(std::complex<float>* a, std::complex<float>* b, std::complex<float>* c, size_t n) {
    size_t i = 0;

    if (n >= 2) {
        for ( ; i < n; i += 2) {
            v128 _a = vld1q_f32(reinterpret_cast<float *>(a + i));
            v128 _b = vld1q_f32(reinterpret_cast<float *>(b + i));
            v128 _c = vcdivq_f32(_a, _b);

            vst1q_f32(reinterpret_cast<float *>(c + i), _c);
        }
    }

    for ( ; i < n; ++i) {
        c[i] = a[i] / b[i];
    }
}

void dilate(float* a, float* b, size_t n, size_t d) {
    size_t i = 0;

    RACS_UNROLL(8)
    for ( ; i < n; ++i) {
        b[i * d] = a[i];
    }
}

void interleaved(const float *q0, const float *q1, float *q2, size_t n) {
    size_t i = 0;
    size_t m = n & ~3;

    for (; i < m; i += 2) {
        v128 _q0 = vld1q_f32(q0 + i);
        v128 _q1 = vld1q_f32(q1 + i);
        v128 _q2 = vzip1q_f32(_q0, _q1);
        vst1q_f32(q2 + i * 2, _q2);
    }

    for (; i < n; ++i) {
        q2[i * 2] = q0[i];
        q2[i * 2 + 1] = q1[i];
    }
}
#endif

}
