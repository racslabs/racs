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

#ifndef RACS_ALLOCATOR_HPP
#define RACS_ALLOCATOR_HPP

#include <limits>
#include <new>
#include <utility>
#include <iostream>
#include <complex>

#include "../simd/constants.hpp"

namespace racs {

void* racs_malloc(size_t size);

void racs_free(void* ptr);

size_t nextpow2(size_t x);

/**
 * @tparam T Data type of elements to allocate.
 */
template<typename T>
struct aligned_allocator {
    using value_type = T;
    using pointer = value_type *;

    aligned_allocator() noexcept = default;

    template<typename U>
    explicit aligned_allocator(const aligned_allocator<U> &) noexcept {};

    /**
     * @brief Allocates a block of memory aligned to a 32-byte boundary.
     *
     * @param size The number of elements to allocate.
     * @return A pointer to the allocated memory block.
     */
    T *allocate(size_t size);

    /**
     * @brief Deallocates aligned memory block.
     * @param ptr A pointer to the memory block to deallocate.
     */
    void deallocate(T *ptr);
};

/**
 * @brief Provides additional functions and traits for `racs::aligned_allocator`.
 * @tparam A The allocator type.
 */
template<typename A>
struct aligned_allocator_traits {
    using allocator_type = A;
    using value_type = typename A::value_type;
    using pointer = typename A::pointer;

    /**
     * @brief Allocates memory using `racs::aligned_allocator`.
     * @param allocator The allocator to use.
     * @param n The number of elements to allocate.
     * @return A pointer to the allocated memory block.
     */
    static pointer allocate(allocator_type &allocator, size_t n) {
        return allocator.allocate(n);
    }

    /**
      * @brief Deallocates aligned memory block using `racs::aligned_allocator`.
      * @param allocator The allocator to use.
      * @param ptr A pointer to the memory block to deallocate.
      */
    static void deallocate(allocator_type &allocator, pointer ptr) {
        return allocator.deallocate(ptr);
    }

    template<typename... Args>
    static void construct(allocator_type &allocator, pointer ptr, Args &&...args) {
        ::new(ptr) value_type(std::forward<Args>(args)...);
    }

    static void destroy(allocator_type &allocator, pointer ptr) {
        ptr->~value_type();
    }

};

}
#endif //RACS_ALLOCATOR_HPP
