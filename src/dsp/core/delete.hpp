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

#ifndef RACS_DELETE_HPP
#define RACS_DELETE_HPP

#include "tensor.hpp"

namespace racs {

    /**
    * @defgroup del
    * @{
    */

    /**
     * @tparam T The type of the tensor elements.
     * @param x The 1-D input tensor.
     * @param idx The index of the element to delete.
     * @return A new 1-D tensor with the element at the specified index removed.
     *
     * Example: <br/>
     * Remove an element from a 1-D tensor.
     * @code{cpp}
     * racs::tensor<1, float> x = {1., 2., 3., 4.};
     *
     * // returns {1., 3., 4.}
     * auto y = racs::del(x, 1);
     * @endcode
     */
    template<typename T>
    tensor<1, T> del(const tensor<1, T>& x, size_t idx);

    /**
     * @tparam T The type of the real and imaginary parts of the tensor elements.
     * @param x The complex 1-D input tensor.
     * @param idx The index of the element to delete.
     * @return A new complex 1-D tensor with the element at the specified index removed.
     *
     * Example: <br/>
     * Remove an element from a complex 1-D tensor.
     * @code{cpp}
     * racs::tensor<1, std::complex<float>> x = {{1., 0.}, {2., 0.}, {3., 0.}, {4., 0.}};
     *
     * // returns {{1., 0.}, {3., 0.}, {4., 0.}}
     * auto y = racs::del(x, 1);
     * @endcode
     */
    template<typename T>
    tensor<1, std::complex<T>> del(const tensor<1, std::complex<T>>& x, size_t idx);

    /** @} */

}

#endif //RACS_DELETE_HPP
