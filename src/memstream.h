#ifndef RACS_MEMSTREAM_H
#define RACS_MEMSTREAM_H


#ifdef __cplusplus
extern "C" {
#endif


#include <string.h>
#include <stdlib.h>
#include "types.h"


typedef struct {
    racs_uint8 **data;
    size_t *size;
    size_t capacity;
} racs_memstream;


int racs_memstream_write(racs_memstream *ms, const void *src, size_t len);


#ifdef __cplusplus
}
#endif

#endif //RACS_MEMSTREAM_H
