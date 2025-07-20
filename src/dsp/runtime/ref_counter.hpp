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

#ifndef RACS_REF_COUNTER_HPP
#define RACS_REF_COUNTER_HPP

#include "../core/allocator.hpp"

namespace racs {

class ref_counter {
    using allocator_type = aligned_allocator<size_t>;
    using allocator_traits = aligned_allocator_traits<allocator_type>;

public:
    ref_counter();

    ref_counter(size_t* count);

    ~ref_counter();

    size_t* count() const;

private:
    allocator_type allocator_;

    size_t* count_;
};

}

#endif //RACS_REF_COUNTER_HPP
