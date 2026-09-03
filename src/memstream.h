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
    off_t  offset;
} racs_memstream;


int racs_memstream_write(racs_memstream *ms, const void *src, size_t len);

int racs_memstream_seek(racs_memstream *ms, size_t offset);

off_t racs_memstream_tell(racs_memstream *ms);


#ifdef __cplusplus
}
#endif

#endif //RACS_MEMSTREAM_H
