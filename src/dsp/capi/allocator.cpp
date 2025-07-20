#include "allocator.h"
#include "../core/allocator.hpp"

void* racs_malloc(size_t size) {
    return racs::racs_malloc(size);
}

void racs_free(void *ptr) {
    return racs::racs_free(ptr);
}
