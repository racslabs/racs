
#ifndef AUXTS_STREAM_H
#define AUXTS_STREAM_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "type.h"

typedef enum {
    AUXTS_MEMORY_STREAM_CONTINUE,
    AUXTS_MEMORY_STREAM_ABORT
} auxts_memory_stream_status;

typedef struct {
    size_t size;
    size_t current_pos;
    auxts_uint8* data;
} auxts_memory_stream;

void auxts_memory_stream_init(auxts_memory_stream* stream, void* data, size_t size);
int auxts_memory_stream_write(auxts_memory_stream* stream, const void* data, size_t size);

#endif //AUXTS_STREAM_H
