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

#include "wavelets.hpp"

namespace racs::dwt {

template<typename T>
tensor<2, T> make_filter_bank(const tensor<1, T>& phi) {
    auto n = phi.size();

    tensor<2, T> filter_bank(tensor_shape({ 4, n }));

    auto lo_R = phi;
    put(filter_bank, 2, lo_R);

    auto lo_D = reverse(lo_R);
    put(filter_bank, 0, lo_D);

    auto _lo_D = bitcast<std::complex<T>>(lo_D);
    auto _hi_R = math::cconj(_lo_D);

    auto hi_R = bitcast<T>(_hi_R);
    put(filter_bank, 3, hi_R);

    auto hi_D = reverse(hi_R);
    put(filter_bank, 1, hi_D);

    return filter_bank;
}

template<typename T>
wavelet<T>::wavelet(WAVELET name) {
    initialize(name);
}

template<typename T>
tensor<2, T> wavelet<T>::filter_bank() const {
    return filter_bank_;
}

template<typename T>
void wavelet<T>::initialize(WAVELET name) {
    switch (name) {
        case WAVELET::DB1:
            filter_bank_ = make_filter_bank(_db1<T>());
            break;
        case WAVELET::DB2:
            filter_bank_ = make_filter_bank(_db2<T>());
            break;
        case WAVELET::DB3:
            filter_bank_ = make_filter_bank(_db3<T>());
            break;
        case WAVELET::DB4:
            filter_bank_ = make_filter_bank(_db4<T>());
            break;
        case WAVELET::DB5:
            filter_bank_ = make_filter_bank(_db5<T>());
            break;
        case WAVELET::DB6:
            filter_bank_ = make_filter_bank(_db6<T>());
            break;
        case WAVELET::DB7:
            filter_bank_ = make_filter_bank(_db7<T>());
            break;
        case WAVELET::DB8:
            filter_bank_ = make_filter_bank(_db8<T>());
            break;
        case WAVELET::DB9:
            filter_bank_ = make_filter_bank(_db9<T>());
            break;
        case WAVELET::DB10:
            filter_bank_ = make_filter_bank(_db10<T>());
            break;
        case WAVELET::DB11:
            filter_bank_ = make_filter_bank(_db11<T>());
            break;
        case WAVELET::DB12:
            filter_bank_ = make_filter_bank(_db12<T>());
            break;
        case WAVELET::DB13:
            filter_bank_ = make_filter_bank(_db13<T>());
            break;
        case WAVELET::DB14:
            filter_bank_ = make_filter_bank(_db14<T>());
            break;
        case WAVELET::DB15:
            filter_bank_ = make_filter_bank(_db15<T>());
            break;
        case WAVELET::DB16:
            filter_bank_ = make_filter_bank(_db16<T>());
            break;
        case WAVELET::DB17:
            filter_bank_ = make_filter_bank(_db17<T>());
            break;
        case WAVELET::DB18:
            filter_bank_ = make_filter_bank(_db18<T>());
            break;
        case WAVELET::DB19:
            filter_bank_ = make_filter_bank(_db19<T>());
            break;
        case WAVELET::DB20:
            filter_bank_ = make_filter_bank(_db20<T>());
            break;
        case WAVELET::SYM2:
            filter_bank_ = make_filter_bank(_sym2<T>());
            break;
        case WAVELET::SYM3:
            filter_bank_ = make_filter_bank(_sym3<T>());
            break;
        case WAVELET::SYM4:
            filter_bank_ = make_filter_bank(_sym4<T>());
            break;
        case WAVELET::SYM5:
            filter_bank_ = make_filter_bank(_sym5<T>());
            break;
        case WAVELET::SYM6:
            filter_bank_ = make_filter_bank(_sym6<T>());
            break;
        case WAVELET::SYM7:
            filter_bank_ = make_filter_bank(_sym7<T>());
            break;
        case WAVELET::SYM8:
            filter_bank_ = make_filter_bank(_sym8<T>());
            break;
        case WAVELET::SYM9:
            filter_bank_ = make_filter_bank(_sym9<T>());
            break;
        case WAVELET::SYM10:
            filter_bank_ = make_filter_bank(_sym10<T>());
            break;
        case WAVELET::SYM11:
            filter_bank_ = make_filter_bank(_sym11<T>());
            break;
        case WAVELET::SYM12:
            filter_bank_ = make_filter_bank(_sym12<T>());
            break;
        case WAVELET::SYM13:
            filter_bank_ = make_filter_bank(_sym13<T>());
            break;
        case WAVELET::SYM14:
            filter_bank_ = make_filter_bank(_sym14<T>());
            break;
        case WAVELET::SYM15:
            filter_bank_ = make_filter_bank(_sym15<T>());
            break;
        case WAVELET::SYM16:
            filter_bank_ = make_filter_bank(_sym16<T>());
            break;
        case WAVELET::SYM17:
            filter_bank_ = make_filter_bank(_sym17<T>());
            break;
        case WAVELET::SYM18:
            filter_bank_ = make_filter_bank(_sym18<T>());
            break;
        case WAVELET::SYM19:
            filter_bank_ = make_filter_bank(_sym19<T>());
            break;
        case WAVELET::SYM20:
            filter_bank_ = make_filter_bank(_sym20<T>());
            break;
        case WAVELET::COIF1:
            filter_bank_ = make_filter_bank(_coif1<T>());
            break;
        case WAVELET::COIF2:
            filter_bank_ = make_filter_bank(_coif2<T>());
            break;
        case WAVELET::COIF3:
            filter_bank_ = make_filter_bank(_coif3<T>());
            break;
        case WAVELET::COIF4:
            filter_bank_ = make_filter_bank(_coif4<T>());
            break;
        case WAVELET::COIF5:
            filter_bank_ = make_filter_bank(_coif5<T>());
            break;
        case WAVELET::COIF6:
            filter_bank_ = make_filter_bank(_coif6<T>());
            break;
        case WAVELET::COIF7:
            filter_bank_ = make_filter_bank(_coif7<T>());
            break;
        case WAVELET::COIF8:
            filter_bank_ = make_filter_bank(_coif8<T>());
            break;
        case WAVELET::COIF9:
            filter_bank_ = make_filter_bank(_coif9<T>());
            break;
        case WAVELET::COIF10:
            filter_bank_ = make_filter_bank(_coif10<T>());
            break;
        case WAVELET::COIF11:
            filter_bank_ = make_filter_bank(_coif11<T>());
            break;
        case WAVELET::COIF12:
            filter_bank_ = make_filter_bank(_coif12<T>());
            break;
        case WAVELET::COIF13:
            filter_bank_ = make_filter_bank(_coif13<T>());
            break;
        case WAVELET::COIF14:
            filter_bank_ = make_filter_bank(_coif14<T>());
            break;
        case WAVELET::COIF15:
            filter_bank_ = make_filter_bank(_coif15<T>());
            break;
        case WAVELET::COIF16:
            filter_bank_ = make_filter_bank(_coif16<T>());
            break;
        case WAVELET::COIF17:
            filter_bank_ = make_filter_bank(_coif17<T>());
            break;
        case WAVELET::BIOR1_0:
            filter_bank_ = make_filter_bank(_bior1_0<T>());
            break;
        case WAVELET::BIOR1_1:
            filter_bank_ = make_filter_bank(_bior1_1<T>());
            break;
        case WAVELET::BIOR1_3:
            filter_bank_ = make_filter_bank(_bior1_3<T>());
            break;
        case WAVELET::BIOR1_5:
            filter_bank_ = make_filter_bank(_bior1_5<T>());
            break;
        case WAVELET::BIOR2_0:
            filter_bank_ = make_filter_bank(_bior2_0<T>());
            break;
        case WAVELET::BIOR2_2:
            filter_bank_ = make_filter_bank(_bior2_2<T>());
            break;
        case WAVELET::BIOR2_4:
            filter_bank_ = make_filter_bank(_bior2_4<T>());
            break;
        case WAVELET::BIOR2_6:
            filter_bank_ = make_filter_bank(_bior2_6<T>());
            break;
        case WAVELET::BIOR2_8:
            filter_bank_ = make_filter_bank(_bior2_8<T>());
            break;
        case WAVELET::BIOR3_0:
            filter_bank_ = make_filter_bank(_bior3_0<T>());
            break;
        case WAVELET::BIOR3_1:
            filter_bank_ = make_filter_bank(_bior3_1<T>());
            break;
        case WAVELET::BIOR3_3:
            filter_bank_ = make_filter_bank(_bior3_3<T>());
            break;
        case WAVELET::BIOR3_5:
            filter_bank_ = make_filter_bank(_bior3_5<T>());
            break;
        case WAVELET::BIOR3_7:
            filter_bank_ = make_filter_bank(_bior3_7<T>());
            break;
        case WAVELET::BIOR3_9:
            filter_bank_ = make_filter_bank(_bior3_9<T>());
            break;
        case WAVELET::BIOR4_0:
            filter_bank_ = make_filter_bank(_bior4_0<T>());
            break;
        case WAVELET::BIOR4_4:
            filter_bank_ = make_filter_bank(_bior4_4<T>());
            break;
        case WAVELET::BIOR5_0:
            filter_bank_ = make_filter_bank(_bior5_0<T>());
            break;
        case WAVELET::BIOR5_5:
            filter_bank_ = make_filter_bank(_bior5_5<T>());
            break;
        case WAVELET::BIOR6_0:
            filter_bank_ = make_filter_bank(_bior6_0<T>());
            break;
        case WAVELET::BIOR6_8:
            filter_bank_ = make_filter_bank(_bior6_8<T>());
            break;
        case WAVELET::DMEY:
            filter_bank_ = make_filter_bank(_dmey<T>());
            break;
    }
}

template class wavelet<float>;

}