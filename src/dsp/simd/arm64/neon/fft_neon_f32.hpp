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

#ifndef RACS_SIMD_FFT_NEON_F32_CPP
#define RACS_SIMD_FFT_NEON_F32_CPP

#include "../../fft.hpp"

namespace racs::simd {

#ifdef __ARM_NEON__

RACS_SIMD v128i RACS_VECTORCALL vbitreverseq_s32(v128i _a, int m) {
    const v128i _mask = vdupq_n_s32(0x1);
    v128i _b = vdupq_n_s32(0);

    for (size_t i = 0; i < m; i++) {
        _b = vshlq_n_s32(_b, 0x1);
        _b = vorrq_s32(_b, vandq_s32(_a, _mask));
        _a = vshrq_n_s32(_a, 0x1);
    }

    return _b;
}
#endif
}

#endif //RACS_SIMD_FFT_NEON_F32_CPP
