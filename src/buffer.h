
#ifndef AUXTS_BUFFER_H
#define AUXTS_BUFFER_H

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

#endif //AUXTS_BUFFER_H
