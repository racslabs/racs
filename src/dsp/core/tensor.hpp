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

#ifndef RACS_TENSOR_HPP
#define RACS_TENSOR_HPP

#include <iostream>
#include <initializer_list>
#include <utility>
#include <vector>
#include <complex>
#include <numeric>

#include "shape.hpp"
#include "allocator.hpp"
#include "../runtime/exceptions.hpp"
#include "../runtime/ref_counter.hpp"

namespace racs {

#define RACS_CHECK_EQUAL_DIMS(x, NDIMS) \
    std::stringstream ss;                \
    if (shape_.ndims() != NDIMS && shape_.ndims() != 0) { \
        ss << shape_ << "does not have " << NDIMS << " dims"; \
        throw runtime::zf100_exception(ss.str());             \
    }

template<typename U>
using initializer_list1d = std::initializer_list<U>;

template<typename U>
using initializer_list2d = initializer_list1d<initializer_list1d<U>>;

template<typename U>
using initializer_list3d = initializer_list2d<initializer_list1d<U>>;

template<typename U>
using initializer_list4d = initializer_list3d<initializer_list1d<U>>;

/**
 * @brief Templated class representing an n-dimensional dense array.
 * @tparam NDIMS Number of dimensions
 * @tparam T Data type of the elements
 * @note `float` and `std::complex<float>` currently are the only data types supported for template parameter `T`.
 */

template<int NDIMS, typename T>
class tensor : public ref_counter {

public:
    using allocator_type = aligned_allocator<T>;
    using allocator_traits = aligned_allocator_traits<allocator_type>;

    using value_type = T;
    using pointer = value_type*;

    ~tensor();

    tensor() = default;

    /**
     *
     * @brief Constructs a tensor from a 1D initializer list
     * @param list 1-D Initializer list
     *
     * Example:<br/>
     * Initialize 1-D float tensor.
     * @code{cpp}
     * racs::tensor<1, float> x = {1., 2., 3.};
     * @endcode
     */
    tensor(const initializer_list1d<T>& list);

    /**
     *
     * @brief Constructs a tensor a 2D initializer list
     * @param list 2-D Initializer list
     *
     * Example:<br/>
     * Initialize 2-D float tensor.
     * @code{cpp}
     * racs::tensor<2, float> x = {{1., 2.}, {3., 4.}};
     * @endcode
     */
    tensor(const initializer_list2d<T>& list);

    /**
     *
     * @brief Constructs a tensor from a 3D initializer list
     * @tparam NDIMS Number of dimensions
     * @tparam T Data type
     * @param list 3-D Initializer list
     *
     * Example:<br/>
     * Initialize 3-D float tensor.
     * @code{cpp}
     * racs::tensor<3, float> x = {
     *      {{1., 2.}, {3., 4.}},
     *      {{5., 6.}, {7., 8.}}
     * };
     * @endcode
     */
    tensor(const initializer_list3d<T>& list);

    /**
     *
     * @brief Constructs a tensor a 4D initializer list
     * @param list 4-D Initializer list
     *
     * Example:<br/>
     * Initialize 4-D float tensor.
     * @code{cpp}
     * racs::tensor<4, float> x = {
     *      {
     *          {{1., 2.}, {3., 4.}},
     *          {{5., 6.}, {7., 8.}}
     *      },
     *      {
     *          {{9., 10.}, {11., 12.}},
     *          {{13., 14.}, {15., 16.}}
     *      }
     * };
     * @endcode
     */
    tensor(const initializer_list4d<T>& list);

    /**
     * @brief Constructs a tensor with the specified shape.
     * @param shape The shape of the tensor.
     *
     * Example:<br/>
     * Initialize a 2-D tensor with shape (3, 2).
     * @code{cpp}
     * racs::tensor<2, float> x(racs::tensor_shape({3, 2});
     * @endcode
     */
    explicit tensor(const tensor_shape& shape);

    tensor(const tensor_shape& shape, T* data, size_t* count);

    /**
     *
     * @param shape The shape of the tensor.
     * @param data A raw data pointer.
     * @warning The `data` pointer must have proper memory alignment.
     *
     * Example:<br/>
     * Create a 2-D float tensor with shape (2, 2) from a raw data pointer.
     * @code{cpp}
     * // aligned to 32-byte boundary
     * float* data = _mm_malloc(sizeof(float) * 4, 32);
     * data[0] = 1;
     * data[1] = 2;
     * data[2] = 3;
     * data[3] = 4;
     *
     * // {{1, 2}, {3, 4}}
     * racs::tensor<2, float> x(racs::tensor_shape({2, 2}), data);
     * @endcode
     */
    tensor(const tensor_shape& shape, T* data);

    tensor(const tensor<NDIMS, T>& other);

    tensor(tensor<NDIMS, T>&& other) noexcept;

    /**
     * @brief Populates the tensor with a scalar value.
     * @param value A scalar value of type `T`.
     */
    void fill(T value) const;

    /**
     * @brief Detaches ownership of the raw data pointer from the tensor.
     * @warning Once this method is called, `racs::ref_counter` will no longer track the lifetime of the data pointer.
     * @return Raw data pointer of the tensor.
     */
    pointer detach();

    /**
     *
     * @return Shape of the tensor.
     */
    [[nodiscard]]
    tensor_shape shape() const;

    /**
     *
     * @return Raw data pointer of the tensor.
     */
    pointer data() const;

    /**
     *
     * @return Size of the tensor.
     */
    [[nodiscard]]
    size_t size() const;

    [[nodiscard]]
    std::vector<size_t> strides() const;

    template <typename... Idx>
    RACS_INLINE value_type& operator()(Idx... indices) noexcept {
        static_assert(sizeof...(Idx) == NDIMS);
        return data_[index(indices...)];
    }

    template <typename... Idx>
    RACS_INLINE value_type operator()(Idx... indices) const noexcept {
        static_assert(sizeof...(Idx) == NDIMS);
        return data_[index(indices...)];
    }

    RACS_INLINE value_type& operator[](size_t idx) noexcept {
        return data_[idx];
    }

    RACS_INLINE value_type operator[](size_t idx) const noexcept {
        return data_[idx];
    }

    RACS_INLINE tensor<NDIMS, T>& operator=(const tensor<NDIMS, T>& other) {
        if (this != &other) {
            allocator_traits::deallocate(allocator_, data_);

            wrapped_ = false;
            shape_ = other.shape_;
            strides_ = other.strides_;
            size_ = other.size_;
            allocator_ = other.allocator_;

            data_ = allocator_traits::allocate(allocator_, size_);
            memcpy(data_, other.data_, sizeof(T) * size_);
        }

        return *this;
    }

    template<typename ...Idx>
    RACS_INLINE size_t index(size_t i, Idx ...indices) const {
        return strides_[(NDIMS - 1) - sizeof...(Idx)] * i + index(indices...);
    }

    RACS_INLINE size_t index() const {
        return 0;
    }

private:
    bool wrapped_{};

    tensor_shape shape_{};

    size_t size_{};

    allocator_type allocator_{};

    std::vector<size_t> strides_;

    pointer data_;

    static size_t get_size_from_shape(const tensor_shape& shape);

    std::vector<size_t> get_strides_from_shape(const tensor_shape& shape);

    void recursive_copy_initializer_list(pointer data, const initializer_list1d<T>& list);

    void recursive_copy_initializer_list(pointer data, const initializer_list2d<T>& list);

    void recursive_copy_initializer_list(pointer data, const initializer_list3d<T>& list);

    void recursive_copy_initializer_list(pointer data, const initializer_list4d<T>& list);

    tensor_shape get_shape_from_initializer_list(const initializer_list1d<T> &list);

    tensor_shape get_shape_from_initializer_list(const initializer_list2d<T> &list);

    tensor_shape get_shape_from_initializer_list(const initializer_list3d<T> &list);

    tensor_shape get_shape_from_initializer_list(const initializer_list4d<T> &list);

};

}

#endif //RACS_TENSOR_HPP
