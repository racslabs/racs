#include "stream.h"

void auxts_memory_stream_init(auxts_memory_stream* stream, void* data, size_t size) {
    memset(stream, 0, sizeof(auxts_memory_stream));
    stream->size = size;
    stream->data = (uint8_t*)data;
}

int auxts_memory_stream_write(auxts_memory_stream* stream, void* data, size_t size) {
    if (stream->current_pos + size >= stream->size) {
        stream->size = (size + stream->size) * 2;

        const auxts_uint8* s_data = realloc(stream->data, stream->size);
        if (!s_data) {
            perror("Failed to re-allocate auxts_memory_stream data");
            return AUXTS_MEMORY_STREAM_ABORT;
        }

        stream->data = (auxts_uint8*)s_data;
    }

    memcpy(stream->data + stream->current_pos, data, size);

    return AUXTS_MEMORY_STREAM_CONTINUE;
}