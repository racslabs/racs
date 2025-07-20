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

#include "allocator.hpp"

namespace racs {

size_t nextpow2(size_t x) {
    x -= 1;
    x |= (x >> 1);
    x |= (x >> 2);
    x |= (x >> 4);
    x |= (x >> 8);
    x |= (x >> 16);
    return x + 1;
}

void* racs_malloc(size_t size) {
    #if defined(__SSE4_1__) || defined(__AVX2__)
        return _mm_malloc(size, RACS_ALIGN);
    #elif defined(__MSVC__)
        return _aligned_malloc(size, RACS_ALIGN);
    #else
        void* aligned_ptr;

        posix_memalign(&aligned_ptr, RACS_ALIGN, size);
        return aligned_ptr;
    #endif
}

void racs_free(void* ptr) {
    #if defined(__SSE4_1__) || defined(__AVX2__)
        _mm_free(ptr);
    #elif defined(__MSVC__)
        _aligned_free(ptr);
    #else
        free(ptr);
    #endif
}

template<typename T>
T *aligned_allocator<T>::allocate(size_t size) {
    auto* ptr = static_cast<T *>(racs_malloc(sizeof(T) * nextpow2(size)));

    if (ptr != nullptr) {
        return ptr;
    }

    if (size == 0) {
        return nullptr;
    }

    throw std::bad_alloc();
}

template<typename T>
void aligned_allocator<T>::deallocate(T *ptr) {
    if (ptr != nullptr) {
        racs_free(ptr);
    }
}

template struct aligned_allocator<float>;
template struct aligned_allocator<double>;
template struct aligned_allocator<std::complex<float>>;
template struct aligned_allocator<size_t>;

}