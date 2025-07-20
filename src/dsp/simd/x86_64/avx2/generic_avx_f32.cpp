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

#include "generic_avx_f32.hpp"

namespace racs::simd {

#ifdef __AVX__

void add(float *a, float *b, float *c, size_t n) {
    size_t i = 0;

    if (n >= 8) {
        for (; i < n; i += 8) {
            __m256 _a = _mm256_load_ps(a + i);
            __m256 _b = _mm256_load_ps(b + i);
            __m256 _c = _mm256_add_ps(_a, _b);

            _mm256_store_ps(c + i, _c);
        }
    }

    for ( ; i < n; ++i) {
        c[i] = a[i] + b[i];
    }
}

void sub(float *a, float *b, float *c, size_t n) {
    size_t i = 0;

    if (n >= 8) {
        for (; i < n; i += 8) {
            __m256 _a = _mm256_load_ps(a + i);
            __m256 _b = _mm256_load_ps(b + i);
            __m256 _c = _mm256_sub_ps(_a, _b);

            _mm256_store_ps(c + i, _c);
        }
    }

    for ( ; i < n; ++i) {
        c[i] = a[i] - b[i];
    }
}

void mul(float *a, float *b, float *c, size_t n) {
    size_t i = 0;

    if (n >= 8) {
        for (; i < n; i += 8) {
            __m256 _a = _mm256_load_ps(a + i);
            __m256 _b = _mm256_load_ps(b + i);
            __m256 _c = _mm256_mul_ps(_a, _b);

            _mm256_store_ps(c + i, _c);
        }
    }

    for ( ; i < n; ++i) {
        c[i] = a[i] * b[i];
    }
}

void div(float *a, float *b, float *c, size_t n) {
    size_t i = 0;

    if (n >= 8) {
        for (; i < n; i += 8) {
            __m256 _a = _mm256_load_ps(a + i);
            __m256 _b = _mm256_load_ps(b + i);
            __m256 _c = _mm256_div_ps(_a, _b);

            _mm256_store_ps(c + i, _c);
        }
    }

    for ( ; i < n; ++i) {
        c[i] = a[i] / b[i];
    }
}

void mod(float *a, float *b, float *c, size_t n) {
    size_t i = 0;

    if (n >= 8) {
        for (; i < n; i += 8) {
            __m256 _a = _mm256_load_ps(a + i);
            __m256 _b = _mm256_load_ps(b + i);
            __m256 _c = _mm256_mod_ps(_a, _b);

            _mm256_store_ps(c + i, _c);
        }
    }

    for ( ; i < n; ++i) {
        c[i] = mod(a[i], b[i]);
    }
}

void rev(float* a, float* b, size_t n) {
    size_t i = 0;

    if (n >= 8) {
        for (; i < n; i += 8) {
            __m256 _a = _mm256_loadu_ps(a + i);
            __m256 _b = _mm256_rev_ps(_a);
            _mm256_storeu_ps(b + n - i - 8, _b);
        }
    }

    for ( ; i < n; ++i) {
        b[n - i - 1] = a[i];
    }
}

void crev(std::complex<float>* a, std::complex<float>* b, size_t n) {
    size_t i = 0;

    if (n >= 4) {
        for (; i < n; i += 4) {
            __m256 _a = _mm256_loadu_ps(reinterpret_cast<float*>(a + i));
            __m256 _b = _mm256_crev_ps(_a);
            _mm256_storeu_ps(reinterpret_cast<float*>(b + n - i - 4), _b);
        }
    }

    for ( ; i < n; ++i) {
        b[n - i - 1] = a[i];
    }
}

void cmplxmul(std::complex<float>* a, std::complex<float>* b, std::complex<float>* c, size_t n) {
    size_t i = 0;

    if (n >= 4) {
        for ( ; i < n; i += 4) {
            __m256 _a = _mm256_load_ps(reinterpret_cast<float *>(a + i));
            __m256 _b = _mm256_load_ps(reinterpret_cast<float *>(b + i));
            __m256 _c = _mm256_cmul_ps(_a, _b);

            _mm256_store_ps(reinterpret_cast<float *>(c + i), _c);
        }
    }

    for ( ; i < n; ++i) {
        c[i] = a[i] * b[i];
    }
}

void cmplxdiv(std::complex<float>* a, std::complex<float>* b, std::complex<float>* c, size_t n) {
    size_t i = 0;

    if (n >= 4) {
        for ( ; i < n; i += 4) {
            __m256 _a = _mm256_load_ps(reinterpret_cast<float *>(a + i));
            __m256 _b = _mm256_load_ps(reinterpret_cast<float *>(b + i));
            __m256 _c = _mm256_cdiv_ps(_a, _b);

            _mm256_store_ps(reinterpret_cast<float *>(c + i), _c);
        }
    }

    for ( ; i < n; ++i) {
        c[i] = a[i] / b[i];
    }
}

void dilate(float* a, float* b, size_t n, size_t d) {
    size_t i = 0;

    if (d == 2 && n >= 4) {
        for ( ; i < n; i += 4) {
            __m128 _a = _mm_loadu_ps(a + i);
            __m256 _b = _mm256_dilatex2_ps(_a);
            _mm256_storeu_ps(b + i * 2, _b);
        }
    }

    RACS_UNROLL(8)
    for ( ; i < n; ++i) {
        b[i * d] = a[i];
    }
}

#endif

}
