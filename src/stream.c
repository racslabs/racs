#include "stream.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define auxts_max(a, b) ((a) > (b) ? (a) : (b))

void auxts_memory_stream_init(auxts_memory_stream* stream, void* data, size_t size) {
    memset(stream, 0, sizeof(auxts_memory_stream));
    stream->size = size;
    stream->data = data;
}

int auxts_memory_stream_write(auxts_memory_stream* stream, const void* data, size_t size) {
    size_t pos = stream->current_pos + size;

    if (pos > stream->size) {
        size_t new_size = auxts_max(pos, stream->size * 2);

        auxts_uint8* new_data = realloc(stream->data, new_size);
        if (!new_data) {
            perror("Failed to re-allocate auxts_memory_stream data");
            return AUXTS_MEMORY_STREAM_ABORT;
        }

        stream->data = new_data;
        stream->size = new_size;
    }

    memcpy(stream->data + stream->current_pos, data, size);
    stream->current_pos += size;

    return AUXTS_MEMORY_STREAM_CONTINUE;
}

