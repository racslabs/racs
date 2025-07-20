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

#include "wavedec.hpp"

namespace racs::dwt {

size_t _wavedec_max_depth(size_t n, size_t m) {
    return std::floor(std::log2( n / (m - 1)));
}

template<typename T>
std::vector<tensor<1, T>> wavedec(const tensor<1, T>& in, size_t n, WAVELET name) {
    std::vector<tensor<1, T>> dec;

    wavelet<T> wavelet(name);

    auto filter_bank = wavelet.filter_bank();
    auto lo_D = slice(filter_bank, 0);
    auto hi_D = slice(filter_bank, 1);

    tensor<1, T> _in = in;

    for (int i = 0; i < n; ++i) {
        auto out = _dwt_base(_in, lo_D, hi_D);

        auto cA = slice(out, 0);
        auto cD = slice(out, 1);

        _in = cA;
        dec.push_back(cD);
    }

    dec.push_back(_in);
    std::reverse(dec.begin(), dec.end());

    return dec;
}

template<typename T>
tensor<1, T> waverec(const std::vector<tensor<1, T>>& dec, WAVELET name) {
    auto n = dec.size() - 1;
    return upcoef(dec, n, name);
}

template<typename T>
tensor<1, T> upcoef(const std::vector<tensor<1, T>>& dec, size_t n, WAVELET name) {
    wavelet<T> wavelet(name);

    auto filter_bank = wavelet.filter_bank();
    auto lo_R = slice(filter_bank, 2);
    auto hi_R = slice(filter_bank, 3);

    auto cA = dec[0];

    for (int i = 0; i < n; ++i) {
        auto cD = dec[i + 1];

        cA = _idwt_base(cA, cD, lo_R, hi_R);
    }

    return cA;
}

template<typename T>
tensor<1, T> detcoef(const std::vector<tensor<1, T>>& dec, size_t n) {
    return dec[dec.size() - n];
}

template std::vector<tensor<1, float>> wavedec(const tensor<1, float>& in, size_t n, WAVELET name);

template tensor<1, float> waverec(const std::vector<tensor<1, float>>& dec, WAVELET name);

template tensor<1, float> upcoef(const std::vector<tensor<1, float>>& dec, size_t n, WAVELET name);

template tensor<1, float> detcoef(const std::vector<tensor<1, float>>& dec, size_t n);

}
