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

#ifndef RACS_WAVELETS_HPP
#define RACS_WAVELETS_HPP

#include <map>
#include <string>
#include "../core/bitcast.hpp"
#include "../core/generic.hpp"
#include "../core/put.hpp"
#include "../math/complex.hpp"
#include "wavelet_coefficients.hpp"

namespace racs::dwt {

enum class WAVELET : unsigned {
    DB1,
    DB2,
    DB3,
    DB4,
    DB5,
    DB6,
    DB7,
    DB8,
    DB9,
    DB10,
    DB11,
    DB12,
    DB13,
    DB14,
    DB15,
    DB16,
    DB17,
    DB18,
    DB19,
    DB20,
    SYM2,
    SYM3,
    SYM4,
    SYM5,
    SYM6,
    SYM7,
    SYM8,
    SYM9,
    SYM10,
    SYM11,
    SYM12,
    SYM13,
    SYM14,
    SYM15,
    SYM16,
    SYM17,
    SYM18,
    SYM19,
    SYM20,
    COIF1,
    COIF2,
    COIF3,
    COIF4,
    COIF5,
    COIF6,
    COIF7,
    COIF8,
    COIF9,
    COIF10,
    COIF11,
    COIF12,
    COIF13,
    COIF14,
    COIF15,
    COIF16,
    COIF17,
    BIOR1_0,
    BIOR1_1,
    BIOR1_3,
    BIOR1_5,
    BIOR2_0,
    BIOR2_2,
    BIOR2_4,
    BIOR2_6,
    BIOR2_8,
    BIOR3_0,
    BIOR3_1,
    BIOR3_3,
    BIOR3_5,
    BIOR3_7,
    BIOR3_9,
    BIOR4_0,
    BIOR4_4,
    BIOR5_0,
    BIOR5_5,
    BIOR6_0,
    BIOR6_8,
    DMEY
};

template<typename T>
tensor<2, T> make_filter_bank(const tensor<1, T>& phi);

template<typename T>
class wavelet {
public:
    explicit wavelet(WAVELET name);

    tensor<2, T> filter_bank() const;

private:
    tensor<2, T> filter_bank_{};

    void initialize(WAVELET name);
};

}

#endif //RACS_WAVELETS_HPP
