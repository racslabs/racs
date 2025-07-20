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

#ifndef RACS_DWT_HPP
#define RACS_DWT_HPP

#include "wavelets.hpp"
#include "../fft/convolution.hpp"

namespace racs::dwt {

/**
 *
 * @tparam T Data type
 * @param in Input signal
 * @param name Wavelet name
 * @return
 */
template<typename T>
tensor<2, T> dwt(const tensor<1, T>& in, WAVELET name);

/**
 *
 * @tparam T Data type
 * @param in Approximation and detail coefficients
 * @param name Wavelet name
 * @return
 */
template<typename T>
tensor<1, T> idwt(const tensor<2, T>& in, WAVELET name);

template<typename T>
tensor<2, T> _dwt_base(const tensor<1, T>& in,
                       const tensor<1, T> &lo_D,
                       const tensor<1, T> &hi_D);

template<typename T>
tensor<1, T> _idwt_base(const tensor<1, T>& cA,
                        const tensor<1, T>& cD,
                        const tensor<1, T>& lo_R,
                        const tensor<1, T>& hi_R);

template<typename T>
tensor<1, T> _upsample(const tensor<1, T>& x);

template<typename T>
tensor<1, T> _downsample(const tensor<1, T>& x);

template<typename T>
tensor<2, T> _dwt_filterbank(const tensor<1, T> &in,
                             const tensor<1, T> &lo_D,
                             const tensor<1, T> &hi_D);

template<typename T>
tensor<2, T> _idwt_filterbank(const tensor<1, T>& cA,
                              const tensor<1, T>& cD,
                              const tensor<1, T>& lo_R,
                              const tensor<1, T>& hi_R);

template<typename T>
tensor<2, T> _dwt_filterbank_multithreaded(const tensor<1, T> &in,
                                           const tensor<1, T> &lo_D,
                                           const tensor<1, T> &hi_D);

template<typename T>
tensor<2, T> _idwt_filterbank_multithreaded(const tensor<1, T>& cA,
                                            const tensor<1, T>& cD,
                                            const tensor<1, T>& lo_R,
                                            const tensor<1, T>& hi_R);

}

#endif //RACS_DWT_HPP
