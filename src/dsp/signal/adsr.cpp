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

#include "adsr.hpp"

namespace racs::signal {

template<typename T>
tensor<1, T> adsr(const tensor<1, T>& params, T fs) {

    size_t pos = 0;
    size_t n = math::sum(params) * fs;

    tensor<1, T> out(tensor_shape({ n }));

    size_t attack_length = fs * params[0];
    auto a0 = T(11.512925465) / attack_length;
    simd::envelope(out.data(), T(0.00001), a0, attack_length);

    pos += attack_length;

    size_t decay_length = fs * params[1];
    auto a1 = T(-0.6931471806) / decay_length;
    simd::envelope(out.data() + pos, T(1), a1, decay_length);

    pos += decay_length;

    size_t sustain_length = fs * params[2];
    std::fill_n(out.data() + pos, sustain_length, T(0.5));

    pos += sustain_length;

    size_t release_length = fs * params[3];
    auto a2 = T(-10.8197782844) / release_length;
    simd::envelope(out.data() + pos, T(0.5), a2, release_length);

    return out;
}

template tensor<1, float> adsr(const tensor<1, float>& params, float fs);

}
