
#ifndef RACS_CAPI_ALLOCATOR_H
#define RACS_CAPI_ALLOCATOR_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>

void* racs_malloc(size_t size);

void racs_free(void* ptr);

#ifdef __cplusplus
}
#endif

#endif //RACS_CAPI_ALLOCATOR_H
