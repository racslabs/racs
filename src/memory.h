
#ifndef AUXTS_MEMORY_H
#define AUXTS_MEMORY_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "types.h"

typedef enum {
    AUXTS_MEMSTREAM_CONTINUE,
    AUXTS_MEMSTREAM_ABORT
} auxts_memstream_status;

typedef struct {
    size_t size;
    size_t current_pos;
    auxts_uint8* data;
} auxts_memstream;

void auxts_memstream_init(auxts_memstream* stream);
int auxts_memstream_write(auxts_memstream* stream, const void* data, size_t size);

#endif //AUXTS_MEMORY_H
