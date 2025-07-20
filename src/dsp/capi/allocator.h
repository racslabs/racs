
#ifndef RACS_ALLOCATOR_H
#define RACS_ALLOCATOR_H

#include "../core/allocator.hpp"

#ifdef __cplusplus
extern "C" {
#endif

RACS_INLINE void* racs_malloc(size_t size) {
    return racs::racs_malloc(size);
}

RACS_INLINE void racs_free(void* ptr) {
    return racs::racs_free(ptr);
}

#ifdef __cplusplus
}
#endif

#endif //RACS_ALLOCATOR_H
