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

#include "ref_counter.hpp"

namespace racs {

ref_counter::ref_counter() :
        allocator_(allocator_type{}),
        count_(allocator_traits::allocate(allocator_, 1)) {
    *count_ = 1;
}

ref_counter::ref_counter(size_t *count) :
        allocator_(allocator_type{}),
        count_(count) {
    (*count_)++;
}

ref_counter::~ref_counter() {
    (*count_)--;

    if (*count_ == 0) {
        allocator_traits::deallocate(allocator_, count_);
    }
}

size_t* ref_counter::count() const {
    return count_;
}

}