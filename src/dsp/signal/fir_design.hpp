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

#ifndef RACS_FIR_DESIGN_HPP
#define RACS_FIR_DESIGN_HPP

#include "windows.hpp"
#include "iir_design.hpp"
#include "../core/op.hpp"
#include "../math/sum.hpp"

namespace racs::signal {

template<typename T>
tensor<1, T> fir(const tensor<1, T>& window, T fc, BTYPE type =  BTYPE::LOWPASS, T fs = 44100);

template<typename T>
tensor<1, T> fir(const tensor<1, T>& window, T f0, T f1, BTYPE type = BTYPE::LOWPASS, T fs = 44100);

template<typename T>
tensor<1, T> fir_lp(const tensor<1, T>& window, T fc, T fs = 44100);

template<typename T>
tensor<1, T> fir_hp(const tensor<1, T>& window, T fc, T fs = 44100);

template<typename T>
tensor<1, T> fir_bp(const tensor<1, T>& window, T f0, T f1, T fs = 44100);

template<typename T>
tensor<1, T> fir_bs(const tensor<1, T>& window, T f0, T f1, T fs = 44100);

}


#endif //RACS_FIR_DESIGN_HPP
