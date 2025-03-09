
#ifndef AUXTS_BYTES_H
#define AUXTS_BYTES_H

#include "export.h"
#include <string.h>
#include <unistd.h>

AUXTS_FORCE_INLINE off_t auxts_write_bin(uint8_t* buf, void* data, int size, off_t offset) {
    memcpy(buf + offset, data, size);
    return offset + size;
}

AUXTS_FORCE_INLINE off_t auxts_write_uint64(uint8_t* buf, uint64_t d, off_t offset) {
    return auxts_write_bin(buf, &d, sizeof(uint64_t), offset);
}

AUXTS_FORCE_INLINE off_t auxts_write_uint32(uint8_t* buf, uint32_t d, off_t offset) {
    return auxts_write_bin(buf, &d, sizeof(uint32_t), offset);
}

AUXTS_FORCE_INLINE off_t auxts_write_uint16(uint8_t* buf, uint16_t d, off_t offset) {
    return auxts_write_bin(buf, &d, sizeof(uint16_t), offset);
}

AUXTS_FORCE_INLINE off_t auxts_read_uint64(uint64_t* d, uint8_t* buf, off_t offset) {
    memcpy(d, buf + offset, sizeof(uint64_t));
    return offset + (off_t)sizeof(uint64_t);
}

AUXTS_FORCE_INLINE off_t auxts_read_uint32(uint32_t* d, uint8_t* buf, off_t offset) {
    memcpy(d, buf + offset, sizeof(uint32_t));
    return offset + (off_t)sizeof(uint32_t);
}

AUXTS_FORCE_INLINE off_t auxts_read_uint16(uint16_t* d, uint8_t* buf, off_t offset) {
    memcpy(d, buf + offset, sizeof(uint16_t));
    return offset + (off_t)sizeof(uint16_t);
}

AUXTS_FORCE_INLINE void auxts_io_read_uint64(uint64_t* d, int fd) {
    read(fd, d, sizeof(uint64_t));
}

#endif //AUXTS_BYTES_H
