#include "memory.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define rats_max(a, b) ((a) > (b) ? (a) : (b))

void rats_memstream_init(rats_memstream* stream) {
    memset(stream, 0, sizeof(rats_memstream));
}

int rats_memstream_write(rats_memstream* stream, const void* data, size_t size) {
    size_t pos = stream->current_pos + size;

    if (pos > stream->size) {
        size_t _size = rats_max(pos, stream->size * 2);

        rats_uint8* _data = realloc(stream->data, _size);
        if (!_data) {
            perror("Failed to re-allocate rats_memstream data");
            return RATS_MEMSTREAM_ABORT;
        }

        stream->data = _data;
        stream->size = _size;
    }

    memcpy(stream->data + stream->current_pos, data, size);
    stream->current_pos += size;

    return RATS_MEMSTREAM_CONTINUE;
}

