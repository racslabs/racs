#include "stream.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define auxts_max(a, b) ((a) > (b) ? (a) : (b))

void auxts_memstream_init(auxts_memstream* stream) {
    memset(stream, 0, sizeof(auxts_memstream));
}

int auxts_memstream_write(auxts_memstream* stream, const void* data, size_t size) {
    size_t pos = stream->current_pos + size;

    if (pos > stream->size) {
        size_t _size = auxts_max(pos, stream->size * 2);

        auxts_uint8* _data = realloc(stream->data, _size);
        if (!_data) {
            perror("Failed to re-allocate auxts_memstream data");
            return AUXTS_MEMSTREAM_ABORT;
        }

        stream->data = _data;
        stream->size = _size;
    }

    memcpy(stream->data + stream->current_pos, data, size);
    stream->current_pos += size;

    return AUXTS_MEMSTREAM_CONTINUE;
}

