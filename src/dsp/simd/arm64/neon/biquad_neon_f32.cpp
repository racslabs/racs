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

#include "biquad_neon_f32.hpp"

namespace racs::simd {

#ifdef __ARM_NEON__

void biquadInit_config(const float* c, Biquad_config_f32* config) {
    float a1    = c[4];
    float a12   = a1 * a1;
    float a13   = a12 * a1;
    float a14   = a13 * a1;
    float a2    = c[5];
    float a22   = a2 * a2;

    v128* c_ = config->c_;

    c_[0] = {      -a1,        a12,     -a13,         a14 };
    c_[1] = {      -a2,      a1*a2,  -a12*a2,      a13*a2 };
    c_[2] = {        0,        -a1,      a12,        -a13 };
    c_[3] = {        0,        -a2,  2*a1*a2,   -3*a12*a2 };
    c_[4] = {        0,          0,      -a1,         a12 };
    c_[5] = {        0,          0,      -a2,     2*a1*a2 };
    c_[6] = {        0,          0,      a22,   -2*a1*a22 };
    c_[7] = {        0,          0,        0,         -a1 };
    c_[8] = {        0,          0,        0,         -a2 };
    c_[9] = {        0,          0,        0,         a22 };
}

void biquadFIR(float* x, float* y, const float *c, size_t n) {
    size_t i = 0;

    v128 _x0 = vld1q_f32(x);
    v128 _x1 = { 0, x[0], x[1], x[2] };
    v128 _x2 = { 0, 0, x[0], x[1] };

    const v128 _b0 = vdupq_n_f32(c[0]);
    const v128 _b1 = vdupq_n_f32(c[1]);
    const v128 _b2 = vdupq_n_f32(c[2]);

    if (n >= 4) {
        i = 4;
        vst1q_f32(y, vbiquadfirq_f32(_x0, _x1, _x2, _b0, _b1, _b2));
    }

    if (n >= 8) {
        for ( ; i < n; i += 4) {
            _x0 = vld1q_f32(x + i);
            _x1 = vld1q_f32(x + i - 1);
            _x2 = vld1q_f32(x + i - 2);

            vst1q_f32(y + i, vbiquadfirq_f32(_x0, _x1, _x2, _b0, _b1, _b2));
        }
    }

    for ( ; i < n; ++i) {
        y[i] = c[0] * x[i] + c[1] * x[i - i] + c[2] * x[i - 2];
    }
}

void biquadIIR(float* y, const float *c, Biquad_config_f32* config, size_t n) {
    size_t i = 0;

    v128* _c = config->c_;
    v128 _wk1 = vdupq_n_f32(0);
    v128 _wk2 = vdupq_n_f32(0);

    v128 _x = vld1q_f32(y + 0);
    v128 _xk = vdupq_n_f32(y[0]);
    v128 _xk1 = vdupq_n_f32(y[1]);
    v128 _xk2 = vdupq_n_f32(y[2]);


    if (n >= 4) {
        i = 4;
        vst1q_f32(y + 0, vbiquadiirq_f32(_x, _xk, _xk1, _xk2, _wk1, _wk2, _c));
    }

    if (n >= 8) {
        for ( ; i < n; i += 4) {
            _wk1 = vdupq_n_f32(y[i - 1]);
            _wk2 = vdupq_n_f32(y[i - 2]);

            _x = vld1q_f32(y + i);
            _xk = vdupq_n_f32(y[i]);
            _xk1 = vdupq_n_f32(y[i + 1]);
            _xk2 = vdupq_n_f32(y[i + 2]);

            vst1q_f32(y + i, vbiquadiirq_f32(_x, _xk, _xk1, _xk2, _wk1, _wk2, _c));
        }
    }

    for ( ; i < n; ++i) {
        y[i] = y[i] + c[4] * y[i - 1] + c[5] * y[i - 2];
    }
}

#endif
}
