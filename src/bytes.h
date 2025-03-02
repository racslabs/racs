
#ifndef AUXTS_BYTES_H
#define AUXTS_BYTES_H

#include "endian.h"
#include <string.h>

AUXTS_FORCE_INLINE off_t auxts_write_bin(uint8_t* buf, void* data, int size, off_t offset) {
    memcpy(buf + offset, data, size);
    return offset + size;
}

AUXTS_FORCE_INLINE off_t auxts_write_uint64(uint8_t* buf, uint64_t d, off_t offset) {
    d = auxts_swap64_if_big_endian(d);
    return auxts_write_bin(buf, &d, sizeof(uint64_t), offset);
}

AUXTS_FORCE_INLINE off_t auxts_write_uint32(uint8_t* buf, uint32_t d, off_t offset) {
    d = auxts_swap32_if_big_endian(d);
    return auxts_write_bin(buf, &d, sizeof(uint32_t), offset);
}

AUXTS_FORCE_INLINE off_t auxts_write_uint16(uint8_t* buf, uint16_t d, off_t offset) {
    d = auxts_swap16_if_big_endian(d);
    return auxts_write_bin(buf, &d, sizeof(uint16_t), offset);
}

AUXTS_FORCE_INLINE off_t auxts_read_uint64(uint64_t* d, uint8_t* buf, off_t offset) {
    memcpy(d, buf + offset, sizeof(uint64_t));
    *d = auxts_swap64_if_big_endian(*d);
    return offset + (off_t)sizeof(uint64_t);
}

AUXTS_FORCE_INLINE off_t auxts_read_uint32(uint32_t* d, uint8_t* buf, off_t offset) {
    memcpy(d, buf + offset, sizeof(uint32_t));
    *d = auxts_swap32_if_big_endian(*d);
    return offset + (off_t)sizeof(uint32_t);
}

AUXTS_FORCE_INLINE off_t auxts_read_uint16(uint16_t* d, uint8_t* buf, off_t offset) {
    memcpy(d, buf + offset, sizeof(uint16_t));
    *d = auxts_swap16_if_big_endian(*d);
    return offset + (off_t)sizeof(uint16_t);
}

AUXTS_FORCE_INLINE void auxts_io_read_uint64(uint64_t* d, int fd) {
    read(fd, d, sizeof(uint64_t));
    *d = auxts_swap64_if_big_endian(*d);
}

/* not sure where to put this? */
AUXTS_FORCE_INLINE uint64_t auxts_next_power_of_two(uint64_t n) {
    n--;
    n |= n >> 1;
    n |= n >> 2;
    n |= n >> 4;
    n |= n >> 8;
    n |= n >> 16;
    n++;

    return n;
}

#endif //AUXTS_BYTES_H
