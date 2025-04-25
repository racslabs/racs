
#ifndef AUXTS_BYTES_H
#define AUXTS_BYTES_H

#include "export.h"
#include "types.h"
#include <string.h>
#include <unistd.h>
#include <stdint.h>

AUXTS_FORCE_INLINE off_t rats_write_bin(rats_uint8* buf, void* data, int size, off_t offset) {
    memcpy(buf + offset, data, size);
    return offset + size;
}

AUXTS_FORCE_INLINE off_t rats_write_uint64(rats_uint8* buf, rats_uint64 d, off_t offset) {
    return rats_write_bin(buf, &d, sizeof(rats_uint64), offset);
}

AUXTS_FORCE_INLINE off_t rats_write_uint32(rats_uint8* buf, rats_uint32 d, off_t offset) {
    return rats_write_bin(buf, &d, sizeof(rats_uint32), offset);
}

AUXTS_FORCE_INLINE off_t rats_write_uint16(rats_uint8* buf, rats_uint16 d, off_t offset) {
    return rats_write_bin(buf, &d, sizeof(rats_uint16), offset);
}

AUXTS_FORCE_INLINE off_t rats_read_uint64(rats_uint64* d, rats_uint8* buf, off_t offset) {
    memcpy(d, buf + offset, sizeof(rats_uint64));
    return offset + (off_t)sizeof(rats_uint64);
}

AUXTS_FORCE_INLINE off_t rats_read_uint32(rats_uint32* d, rats_uint8* buf, off_t offset) {
    memcpy(d, buf + offset, sizeof(rats_uint32));
    return offset + (off_t)sizeof(rats_uint32);
}

AUXTS_FORCE_INLINE off_t rats_read_uint16(rats_uint16* d, rats_uint8* buf, off_t offset) {
    memcpy(d, buf + offset, sizeof(rats_uint16));
    return offset + (off_t)sizeof(rats_uint16);
}

AUXTS_FORCE_INLINE void rats_io_read_uint64(rats_uint64* d, int fd) {
    read(fd, d, sizeof(rats_uint64));
}

#endif //AUXTS_BYTES_H
