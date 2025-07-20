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

#ifndef RACS_SHAPE_HPP
#define RACS_SHAPE_HPP

#include <initializer_list>
#include <string>
#include <ostream>
#include <sstream>
#include <iterator>
#include <iostream>
#include <cstring>
#include <vector>

namespace racs {

using const_tensor_shape_iterator = typename std::vector<size_t>::const_iterator;

class tensor_shape_iterator {

public:
    explicit tensor_shape_iterator(const_tensor_shape_iterator it) :
            iterator_(it) {}

    const size_t& operator*() const {
        return *iterator_;
    }

    tensor_shape_iterator& operator++() {
        ++iterator_;
        return *this;
    }

    tensor_shape_iterator operator++(int) {
        tensor_shape_iterator temp = *this;
        ++(*this);
        return temp;
    }

    bool operator==(const tensor_shape_iterator& other) const {
        return iterator_ == other.iterator_;
    }

    bool operator!=(const tensor_shape_iterator& other) const {
        return !(*this == other);
    }

private:
    const_tensor_shape_iterator iterator_{};
};

class tensor_shape {

public:

    tensor_shape(std::initializer_list<size_t> dims);

    explicit tensor_shape(const std::vector<size_t>& dims);

    tensor_shape_iterator begin() const {
        return tensor_shape_iterator(dims_.begin());
    }

    tensor_shape_iterator end() const {
        return tensor_shape_iterator(dims_.end());
    }

    size_t &operator[](size_t i) noexcept {
        return dims_[i];
    }

    size_t operator[](size_t i) const noexcept {
        return dims_[i];
    }

    friend std::ostream&
    operator<<(std::ostream &os, const tensor_shape &shape) {
        return os << shape.to_string();
    }

    bool operator==(const tensor_shape &shape) const {
        return equals(shape);
    }

    bool operator!=(const tensor_shape &shape) const {
        return !equals(shape);
    }

    size_t ndims() const { return dims_.size(); }

    std::vector<size_t> dims() { return dims_; }

    std::string to_string() const;

    bool is_compatible(const tensor_shape& shape);

    void add_dim(size_t dim);

private:
    std::vector<size_t> dims_{};

    bool equals(const tensor_shape &shape) const;
};

}

#endif //RACS_SHAPE_HPP