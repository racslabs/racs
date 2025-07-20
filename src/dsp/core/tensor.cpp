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

#include "tensor.hpp"

namespace racs {

template<int NDIMS, typename T>
tensor<NDIMS, T>::tensor(const tensor_shape& shape) :
        wrapped_(false),
        shape_(shape),
        strides_(get_strides_from_shape(shape)),
        size_(get_size_from_shape(shape)),
        allocator_(allocator_type{}),
        data_(allocator_traits::allocate(allocator_, size_)),
        ref_counter() {
    RACS_CHECK_EQUAL_DIMS(shape_, NDIMS)
    fill(0);
}

template<int NDIMS, typename T>
tensor<NDIMS, T>::tensor(const tensor_shape &shape, T *data, size_t* count) :
        wrapped_(false),
        shape_(shape),
        strides_(get_strides_from_shape(shape)),
        size_(get_size_from_shape(shape)),
        allocator_(allocator_type{}),
        data_(data),
        ref_counter(count) {
    RACS_CHECK_EQUAL_DIMS(shape_, NDIMS)
}

template<int NDIMS, typename T>
tensor<NDIMS, T>::tensor(const tensor_shape &shape, T *data) :
        wrapped_(true),
        shape_(shape),
        strides_(get_strides_from_shape(shape)),
        size_(get_size_from_shape(shape)),
        allocator_(allocator_type{}),
        data_(data){}

template<int NDIMS, typename T>
tensor<NDIMS, T>::tensor(tensor<NDIMS, T> &&other) noexcept :
        wrapped_(false),
        shape_(other.shape_),
        strides_(other.strides_),
        size_(other.size_),
        allocator_(other.allocator_),
        data_(other.data_),
        ref_counter(other.count()) {
    other.data_ = nullptr;
}

template<int NDIMS, typename T>
tensor<NDIMS, T>::tensor(const tensor<NDIMS, T> &other) :
        wrapped_(false),
        shape_(other.shape_),
        strides_(other.strides_),
        size_(other.size_),
        allocator_(allocator_type{}),
        data_(allocator_traits::allocate(allocator_, size_)),
        ref_counter() {
    memcpy(data_, other.data_, sizeof(T) * size_);
}

template<int NDIMS, typename T>
tensor<NDIMS, T>::tensor(const initializer_list1d<T> &list) : ref_counter() {
    wrapped_ = false;
    shape_ = get_shape_from_initializer_list(list);
    strides_ = get_strides_from_shape(shape_);
    size_ = get_size_from_shape(shape_);
    allocator_ = allocator_type{};
    data_ = allocator_traits::allocate(allocator_, size_);

    recursive_copy_initializer_list(data_, list);
}

template<int NDIMS, typename T>
tensor<NDIMS, T>::tensor(const initializer_list2d<T>& list) : ref_counter() {
    wrapped_ = false;
    shape_ = get_shape_from_initializer_list(list);
    strides_ = get_strides_from_shape(shape_);
    size_ = get_size_from_shape(shape_);
    allocator_ = allocator_type{};
    data_ = allocator_traits::allocate(allocator_, size_);

    recursive_copy_initializer_list(data_, list);
}

template<int NDIMS, typename T>
tensor<NDIMS, T>::tensor(const initializer_list3d<T>& list) : ref_counter() {
    wrapped_ = false;
    shape_ = get_shape_from_initializer_list(list);
    strides_ = get_strides_from_shape(shape_);
    size_ = get_size_from_shape(shape_);
    allocator_ = allocator_type{};
    data_ = allocator_traits::allocate(allocator_, size_);

    recursive_copy_initializer_list(data_, list);
}

template<int NDIMS, typename T>
tensor<NDIMS, T>::tensor(const initializer_list4d<T>& list) : ref_counter() {
    wrapped_ = false;
    shape_ = get_shape_from_initializer_list(list);
    strides_ = get_strides_from_shape(shape_);
    size_ = get_size_from_shape(shape_);
    allocator_ = allocator_type{};
    data_ = allocator_traits::allocate(allocator_, size_);

    recursive_copy_initializer_list(data_, list);
}

template<int NDIMS, typename T>
void tensor<NDIMS, T>::recursive_copy_initializer_list(pointer data, const initializer_list4d<T>& list) {
    size_t i = 0;

    for (auto& _list: list) {
        recursive_copy_initializer_list(data + i, _list);
        i += _list.size() * _list.begin()->size() * _list.begin()->begin()->size();
    }
}

template<int NDIMS, typename T>
void tensor<NDIMS, T>::recursive_copy_initializer_list(pointer data, const initializer_list3d<T> &list) {
    size_t i = 0;

    for (auto& _list: list) {
        recursive_copy_initializer_list(data + i, _list);
        i += _list.size() * _list.begin()->size();
    }
}

template<int NDIMS, typename T>
void tensor<NDIMS, T>::recursive_copy_initializer_list(pointer data, const initializer_list2d<T> &list) {
    size_t i = 0;

    for (auto& _list: list) {
        recursive_copy_initializer_list(data + i, _list);
        i += _list.size();
    }
}

template<int NDIMS, typename T>
void tensor<NDIMS, T>::recursive_copy_initializer_list(pointer data, const initializer_list1d<T> &list) {
    memcpy(data, list.begin(), sizeof(T) * list.size());
}

template<int NDIMS, typename T>
tensor_shape tensor<NDIMS, T>::get_shape_from_initializer_list(const initializer_list1d<T> &list) {
    return tensor_shape({ list.size() });
}

template<int NDIMS, typename T>
tensor_shape tensor<NDIMS, T>::get_shape_from_initializer_list(const initializer_list2d<T> &list) {
    return tensor_shape({ list.size(), list.begin()->size() });
}

template<int NDIMS, typename T>
tensor_shape tensor<NDIMS, T>::get_shape_from_initializer_list(const initializer_list3d<T> &list) {
    return tensor_shape({ list.size(), list.begin()->size(), list.begin()->begin()->size() });
}

template<int NDIMS, typename T>
tensor_shape tensor<NDIMS, T>::get_shape_from_initializer_list(const initializer_list4d<T> &list) {
    return tensor_shape({ list.size(), list.begin()->size(), list.begin()->begin()->size(),
                          list.begin()->begin()->begin()->size() });
}

template<int NDIMS, typename T>
std::vector<size_t> tensor<NDIMS, T>::get_strides_from_shape(const tensor_shape& shape) {
    auto ndims = shape.ndims();
    std::vector<size_t> strides(ndims);
    if (ndims != 0) {
        strides[ndims - 1] = 1;
        for (auto i = ndims - 1; i > 0; --i) {
            strides[i - 1] = strides[i] * shape[i];
        }
    }
    return strides;
}

template<int NDIMS, typename T>
size_t tensor<NDIMS, T>::get_size_from_shape(const tensor_shape &shape) {
    if (shape.ndims() == 0) return 0;
    return std::accumulate(shape.begin(), shape.end(), 1, std::multiplies<>());
}

template<int NDIMS, typename T>
void tensor<NDIMS, T>::fill(const T value) const {
    std::fill_n(data_, size_, value);
}

template<int NDIMS, typename T>
tensor_shape tensor<NDIMS, T>::shape() const {
    return shape_;
}

template<int NDIMS, typename T>
typename tensor<NDIMS, T>::pointer tensor<NDIMS, T>::data() const {
    return data_;
}

template<int NDIMS, typename T>
std::vector<size_t> tensor<NDIMS, T>::strides() const {
    return strides_;
}

template<int NDIMS, typename T>
size_t tensor<NDIMS, T>::size() const {
    return size_;
}

template<int NDIMS, typename T>
typename tensor<NDIMS, T>::pointer tensor<NDIMS, T>::detach()  {
    T* temp = data_;

    data_ = nullptr;
    wrapped_ = true;

    return temp;
}

template<int NDIMS, typename T>
tensor<NDIMS, T>::~tensor() {
    if (!wrapped_) {

        if (*count() == 1) {
            allocator_traits::deallocate(allocator_, data_);
        }
    }
}

#define RACS_REGISTER_TENSOR_TYPE(T) \
    template class tensor<1, T>;      \
    template class tensor<2, T>;      \
    template class tensor<3, T>;      \
    template class tensor<4, T>;

    RACS_REGISTER_TENSOR_TYPE(float)

    RACS_REGISTER_TENSOR_TYPE(double)

    RACS_REGISTER_TENSOR_TYPE(std::complex<float>)

}