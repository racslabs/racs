
#include "memstream.h"


int racs_memstream_write(racs_memstream *ms, const void *src, size_t len) {
    size_t needed = *ms->size + len;

    if (needed > ms->capacity) {
        size_t new_capacity = ms->capacity ? ms->capacity : 64;

        while (new_capacity < needed) {
            new_capacity *= 2;
        }

        void *ptr = realloc(*ms->data, new_capacity);
        if (!ptr) {
            return -1;
        }

        *ms->data = ptr;
        ms->capacity = new_capacity;
    }

    memcpy(*ms->data + *ms->size, src, len);
    *ms->size += len;

    return 0;
}
