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

#include "shape.hpp"

namespace racs {

tensor_shape::tensor_shape(std::initializer_list<size_t> dims) :
        dims_(dims){}

tensor_shape::tensor_shape(const std::vector<size_t> &dims) :
        dims_(dims){}

std::string tensor_shape::to_string() const {
    size_t count = 0;
    std::stringstream ss;
    ss << "(";
    for (auto dim: *this) {
        ss << dim;
        if (count != ndims() - 1) {
            ss << ",";
        }
        count++;
    }
    ss << ")";
    return ss.str();
}

bool tensor_shape::equals(const tensor_shape &shape) const {
    if (ndims() != shape.ndims()) {
        return false;
    }
    return dims_ == shape.dims_;
}

bool tensor_shape::is_compatible(const tensor_shape &shape) {
    return false;
}

void tensor_shape::add_dim(size_t dim) {
    dims_.push_back(dim);
}

}