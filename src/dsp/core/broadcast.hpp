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

#ifndef RACS_BROADCAST_HPP
#define RACS_BROADCAST_HPP

#include "tensor.hpp"
#include "../runtime/exceptions.hpp"

namespace racs {

#define RACS_CHECK_BROADCAST_SHAPE(x, y) \
    std::stringstream ss;                 \
    auto n = x.shape().ndims();           \
    auto m = y.shape().ndims();           \
    if (x.shape()[n - 1] > y.shape()[m - 1]) { \
        ss << "cannot broadcast " << x.shape() << " to " << y.shape(); \
        throw runtime::zf101_exception(ss.str());                       \
    }

    /**
     * @defgroup broadcast_to
     * @{
     */

    /**
     * @brief Broadcast a 1D tensor to match the shape of another 1D tensor.
     * @tparam T The type of the tensor elements.
     * @param src The source tensor to broadcast from.
     * @param dst The destination tensor to broadcast to.
     * @param value The value to fill the broadcasted tensor with.
     * @return The broadcasted tensor.
     */
    template<typename T>
    tensor<1, T> broadcast_to(const tensor<1, T>& src, const tensor<1, T>& dst, T value = 0);

    /**
     * @brief Broadcast a 1D tensor to match the shape of a 2D tensor.
     * @tparam T The type of the tensor elements.
     * @param src The source tensor to broadcast from.
     * @param dst The destination tensor to broadcast to.
     * @param value The value to fill the broadcasted tensor with.
     * @return The broadcasted tensor.
     */
    template<typename T>
    tensor<2, T> broadcast_to(const tensor<1, T>& src, const tensor<2, T>& dst, T value = 0);

    /**
     * @brief Broadcast a 2D tensor to match the shape of another 2D tensor.
     * @tparam T The type of the tensor elements.
     * @param src The source tensor to broadcast from.
     * @param dst The destination tensor to broadcast to.
     * @param value The value to fill the broadcasted tensor with.
     * @return The broadcasted tensor.
     */
    template<typename T>
    tensor<2, T> broadcast_to(const tensor<2, T>& src, const tensor<2, T>& dst, T value = 0);

    /**
     * @brief Broadcast a 1D tensor to match the shape of a 3D tensor.
     * @tparam T The type of the tensor elements.
     * @param src The source tensor to broadcast from.
     * @param dst The destination tensor to broadcast to.
     * @param value The value to fill the broadcasted tensor with.
     * @return The broadcasted tensor.
     */
    template<typename T>
    tensor<3, T> broadcast_to(const tensor<1, T>& src, const tensor<3, T>& dst, T value = 0);

    /**
     * @brief Broadcast a 2D tensor to match the shape of a 3D tensor.
     * @tparam T The type of the tensor elements.
     * @param src The source tensor to broadcast from.
     * @param dst The destination tensor to broadcast to.
     * @param value The value to fill the broadcasted tensor with.
     * @return The broadcasted tensor.
     */
    template<typename T>
    tensor<3, T> broadcast_to(const tensor<2, T>& src, const tensor<3, T>& dst, T value = 0);

    /**
     * @brief Broadcast a 3D tensor to match the shape of another 3D tensor.
     * @tparam T The type of the tensor elements.
     * @param src The source tensor to broadcast from.
     * @param dst The destination tensor to broadcast to.
     * @param value The value to fill the broadcasted tensor with.
     * @return The broadcasted tensor.
     */
    template<typename T>
    tensor<3, T> broadcast_to(const tensor<3, T>& src, const tensor<3, T>& dst, T value = 0);

    /**
     * @brief Broadcast a 1D complex tensor to match the shape of another 1D complex tensor.
     * @tparam T The type of the tensor elements.
     * @param src The source tensor to broadcast from.
     * @param dst The destination tensor to broadcast to.
     * @param value The complex value to fill the broadcasted tensor with.
     * @return The broadcasted complex tensor.
     */
    template<typename T>
    tensor<1, std::complex<T>> broadcast_to(const tensor<1, std::complex<T>>& src,
                                            const tensor<1, std::complex<T>>& dst,
                                            std::complex<T> value = 0);

    /**
     * @brief Broadcast a 1D complex tensor to match the shape of a 2D complex tensor.
     * @tparam T The type of the real and imaginary parts of the tensor elements.
     * @param src The source tensor to broadcast from.
     * @param dst The destination tensor to broadcast to.
     * @param value The complex value to fill the broadcasted tensor with.
     * @return The broadcasted complex tensor.
     */
    template<typename T>
    tensor<2, std::complex<T>> broadcast_to(const tensor<1, std::complex<T>>& src,
                                            const tensor<2, std::complex<T>>& dst,
                                            std::complex<T> value = 0);

    /**
     * @brief Broadcast a 2D complex tensor to match the shape of another 2D complex tensor.
     * @tparam T The type of the tensor elements.
     * @param src The source tensor to broadcast from.
     * @param dst The destination tensor to broadcast to.
     * @param value The complex value to fill the broadcasted tensor with.
     * @return The broadcasted complex tensor.
     */
    template<typename T>
    tensor<2, std::complex<T>> broadcast_to(const tensor<2, std::complex<T>>& src,
                                            const tensor<2, std::complex<T>>& dst,
                                            std::complex<T> value = 0);

    /**
     * @brief Broadcast a 1D complex tensor to match the shape of a 3D complex tensor.
     * @tparam T The type of the real and imaginary parts of the tensor elements.
     * @param src The source tensor to broadcast from.
     * @param dst The destination tensor to broadcast to.
     * @param value The complex value to fill the broadcasted tensor with.
     * @return The broadcasted complex tensor.
     */
    template<typename T>
    tensor<3, std::complex<T>> broadcast_to(const tensor<1, std::complex<T>>& src,
                                            const tensor<3, std::complex<T>>& dst,
                                            std::complex<T> value = 0);

    /**
     * @brief Broadcast a 2D complex tensor to match the shape of a 3D complex tensor.
     * @tparam T The type of the real and imaginary parts of the tensor elements.
     * @param src The source tensor to broadcast from.
     * @param dst The destination tensor to broadcast to.
     * @param value The complex value to fill the broadcasted tensor with.
     * @return The broadcasted complex tensor.
     */
    template<typename T>
    tensor<3, std::complex<T>> broadcast_to(const tensor<2, std::complex<T>>& src,
                                            const tensor<3, std::complex<T>>& dst,
                                            std::complex<T> value = 0);

    /**
     * @brief Broadcast a 3D complex tensor to match the shape of another 3D complex tensor.
     * @tparam T The type of the real and imaginary parts of the tensor elements.
     * @param src The source tensor to broadcast from.
     * @param dst The destination tensor to broadcast to.
     * @param value The complex value to fill the broadcasted tensor with.
     * @return The broadcasted complex tensor.
     */
    template<typename T>
    tensor<3, std::complex<T>> broadcast_to(const tensor<3, std::complex<T>>& src,
                                            const tensor<3, std::complex<T>>& dst,
                                            std::complex<T> value = 0);
    /** @} */
}

#endif //RACS_BROADCAST_HPP
