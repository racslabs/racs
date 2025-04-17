#include "stream.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define auxts_max(a, b) ((a) > (b) ? (a) : (b))

void auxts_memory_stream_init(auxts_memory_stream* stream) {
    memset(stream, 0, sizeof(auxts_memory_stream));
}

int auxts_memory_stream_write(auxts_memory_stream* stream, const void* data, size_t size) {
    size_t pos = stream->current_pos + size;

    if (pos > stream->size) {
        size_t _size = auxts_max(pos, stream->size * 2);

        auxts_uint8* _data = realloc(stream->data, _size);
        if (!_data) {
            perror("Failed to re-allocate auxts_memory_stream data");
            return AUXTS_MEMORY_STREAM_ABORT;
        }

        stream->data = _data;
        stream->size = _size;
    }

    memcpy(stream->data + stream->current_pos, data, size);
    stream->current_pos += size;

    return AUXTS_MEMORY_STREAM_CONTINUE;
}

