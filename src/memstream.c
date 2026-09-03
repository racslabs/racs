
#include "memstream.h"


int racs_memstream_write(racs_memstream *ms, const void *src, size_t len) {
    if (!ms || !ms->data || !(*ms->data) || !src) {
        return -1;
    }

    size_t needed = ms->offset + len;

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

    memcpy(*ms->data + ms->offset, src, len);
    ms->offset += (off_t)len;

    if (ms->offset > *ms->size) {
        *ms->size = ms->offset;
    }

    return 0;
}

int racs_memstream_seek(racs_memstream *ms, size_t offset) {
    if (!ms || !ms->data || !(*ms->data)) {
        return -1;
    }

    if (offset >= ms->capacity) {
        return -1;
    }

    *ms->size = offset;
    return 0;
}

off_t racs_memstream_tell(racs_memstream *ms) {
    if (!ms || !ms->data) {
        return -1;
    }

    return ms->offset;
}