
#ifndef RATS_MEMORY_H
#define RATS_MEMORY_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "types.h"

typedef enum {
    RATS_MEMSTREAM_CONTINUE,
    RATS_MEMSTREAM_ABORT
} rats_memstream_status;

typedef struct {
    size_t size;
    size_t current_pos;
    rats_uint8* data;
} rats_memstream;

void rats_memstream_init(rats_memstream* stream);
int rats_memstream_write(rats_memstream* stream, const void* data, size_t size);

#endif //RATS_MEMORY_H
