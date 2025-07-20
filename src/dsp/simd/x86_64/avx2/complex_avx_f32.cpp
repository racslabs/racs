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

#include "complex_avx_f32.hpp"

namespace racs::simd {

#ifdef __AVX2__

void complex(float* x, std::complex<float>* y, size_t n) {
    size_t i = 0;

    if (n >= 4) {
        for ( ; i < n; i += 4) {
            __m128 _r = _mm_loadu_ps(x + i);
            __m256 _c = _mm256_real2cmplx_ps(_r);
            _mm256_storeu_ps(reinterpret_cast<float *>(y) + i * 2, _c);
        }
    }

    for ( ; i < n; ++i) {
        y[i] = x[i];
    }
}

void cabs(std::complex<float>* x, float* y, size_t n) {
    size_t i = 0;

    if (n >= 4) {
        for (; i < n; i += 4) {
            __m256 _x = _mm256_loadu_ps(reinterpret_cast<float *>(x + i));
            __m128 _y = _mm256_mag_ps(_x);
            _mm_storeu_ps(y + i, _y);
        }
    }

    for ( ; i < n; ++i) {
        y[i] = std::abs(x[i]);
    }
}

void real(std::complex<float>* x, float* y, size_t n) {
    size_t i = 0;

    if (n >= 4) {
        for ( ; i < n; i += 4) {
            __m256 _x = _mm256_loadu_ps(reinterpret_cast<float *>(x + i));
            __m128 _re = _mm256_real_ps(_x);
            _mm_storeu_ps(y + i, _re);
        }
    }

    for ( ; i < n; ++i) {
        y[i] = x[i].real();
    }

}

void imag(std::complex<float>* x, float* y, size_t n) {
    size_t i = 0;

    if (n >= 4) {
        for (; i < n; i += 4) {
            __m256 _x = _mm256_loadu_ps(reinterpret_cast<float *>(x + i));
            __m128 _re = _mm256_img_ps(_x);
            _mm_storeu_ps(y + i, _re);
        }
    }

    for ( ; i < n; ++i) {
        y[i] = x[i].imag();
    }
}

void ccumprod(const std::complex<float> *a, std::complex<float> *b, size_t n) {
    size_t i = 1;

    //TODO: add avx

    b[0] = a[0];
    for ( ; i < n; ++i) {
        b[i] = a[i] * b[i - 1];
    }
}

void cexp(std::complex<float>* a, std::complex<float>* b, size_t n) {
    size_t i = 0;

    if (n >= 4) {
        for ( ; i < n; i += 4) {
            __m256 _a = _mm256_loadu_ps(reinterpret_cast<float*>(a + i));
            __m256 _b = _mm256_cexpz_ps(_a);
            _mm256_storeu_ps(reinterpret_cast<float*>(b + i), _b);
        }
    }

    for ( ; i < n; ++i) {
        b[i] = std::exp(a[i]);
    }
}

void csinh(std::complex<float>* a, std::complex<float>* b, size_t n) {
    size_t i = 0;

    if (n >= 4) {
        for ( ; i < n; i += 4) {
            __m256 _a = _mm256_loadu_ps(reinterpret_cast<float*>(a + i));
            __m256 _b = _mm256_csinh_ps(_a);
            _mm256_storeu_ps(reinterpret_cast<float*>(b + i), _b);
        }
    }

    for ( ; i < n; ++i) {
        b[i] = std::sinh(a[i]);
    }
}

void cconj(std::complex<float>* a, std::complex<float>* b, size_t n) {
    size_t i = 0;

    if (n >= 4) {
        for ( ; i < n; i += 4) {
            __m256 _a = _mm256_loadu_ps(reinterpret_cast<float*>(a + i));
            __m256 _b = _mm256_cconj_ps(_a);
            _mm256_storeu_ps(reinterpret_cast<float*>(b + i), _b);
        }
    }

    for ( ; i < n; ++i) {
        b[i] = std::complex<float>(a[i].real(), -a[i].imag());
    }
}

void csquare(std::complex<float>* a, std::complex<float>* b, size_t n) {
    size_t i = 0;

    if (n >= 4) {
        for ( ; i < n; i += 4) {
            __m256 _a = _mm256_loadu_ps(reinterpret_cast<float*>(a + i));
            __m256 _b = _mm256_csquare_ps(_a);
            _mm256_storeu_ps(reinterpret_cast<float*>(b + i), _b);
        }
    }

    for ( ; i < n; ++i) {
        b[i] = a[i] * a[i];
    }
}

void csqrt(std::complex<float>* a, std::complex<float>* b, size_t n) {
    size_t i = 0;

    if (n >= 4) {
        for ( ; i < n; i += 4) {
            __m256 _a = _mm256_loadu_ps(reinterpret_cast<float*>(a + i));
            __m256 _b = _mm256_csqrt_ps(_a);
            _mm256_storeu_ps(reinterpret_cast<float*>(b + i), _b);
        }
    }

    for ( ; i < n; ++i) {
        b[i] = std::sqrt(a[i]);
    }
}

#endif

}