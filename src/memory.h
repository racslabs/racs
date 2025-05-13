
#ifndef RACS_MEMORY_H
#define RACS_MEMORY_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "types.h"

typedef enum {
    RACS_MEMSTREAM_CONTINUE,
    RACS_MEMSTREAM_ABORT
} racs_memstream_status;

typedef struct {
    size_t size;
    size_t current_pos;
    racs_uint8 *data;
} racs_memstream;

void racs_memstream_init(racs_memstream *stream);

int racs_memstream_write(racs_memstream *stream, const void *data, size_t size);

#endif //RACS_MEMORY_H
