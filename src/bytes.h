// RACS - Remote Audio Caching Server
// Copyright (c) 2025 RACS Labs, LLC. All rights reserved.
//
// Licensed under the RACS Source Available License (RACS-SAL-1.0).
// Non-commercial use only. Commercial use requires a paid license.
// Contact: sales@racslabs.com
//
// SPDX-License-Identifier: RACS-SAL-1.0

#ifndef RACS_BYTES_H
#define RACS_BYTES_H

#ifdef __cplusplus
extern "C" {
#endif

#include "export.h"
#include "types.h"
#include <string.h>
#include <unistd.h>
#include <stdint.h>
#include <arpa/inet.h>


RACS_FORCE_INLINE off_t racs_write_bin(racs_uint8 *buf, void *data, int size, off_t offset) {
    memcpy(buf + offset, data, size);
    return offset + size;
}

RACS_FORCE_INLINE off_t racs_write_uint64(racs_uint8 *buf, racs_uint64 d, off_t offset) {
    return racs_write_bin(buf, &d, sizeof(racs_uint64), offset);
}

RACS_FORCE_INLINE off_t racs_write_uint32(racs_uint8 *buf, racs_uint32 d, off_t offset) {
    return racs_write_bin(buf, &d, sizeof(racs_uint32), offset);
}

RACS_FORCE_INLINE off_t racs_write_uint16(racs_uint8 *buf, racs_uint16 d, off_t offset) {
    return racs_write_bin(buf, &d, sizeof(racs_uint16), offset);
}

RACS_FORCE_INLINE off_t racs_read_uint64(racs_uint64 *d, racs_uint8 *buf, off_t offset) {
    memcpy(d, buf + offset, sizeof(racs_uint64));
    return offset + (off_t) sizeof(racs_uint64);
}

RACS_FORCE_INLINE off_t racs_read_uint32(racs_uint32 *d, racs_uint8 *buf, off_t offset) {
    memcpy(d, buf + offset, sizeof(racs_uint32));
    return offset + (off_t) sizeof(racs_uint32);
}

RACS_FORCE_INLINE off_t racs_read_uint16(racs_uint16 *d, racs_uint8 *buf, off_t offset) {
    memcpy(d, buf + offset, sizeof(racs_uint16));
    return offset + (off_t) sizeof(racs_uint16);
}

RACS_FORCE_INLINE void racs_io_read_uint64(racs_uint64 *d, int fd) {
    read(fd, d, sizeof(racs_uint64));
}

RACS_FORCE_INLINE racs_uint64 racs_htonll(racs_uint64 val) {
    #if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
        return ((racs_uint64)htonl(val & 0xffffffff) << 32) | htonl(val >> 32);
    #else
        return val;
    #endif
}

RACS_FORCE_INLINE racs_uint64 racs_ntohll(racs_uint64 val) {
    #if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
        return ((racs_uint64)ntohl(val & 0xffffffff) << 32) | ntohl(val >> 32);
    #else
        return val;
    #endif
}

#ifdef __cplusplus
}
#endif

#endif //RACS_BYTES_H
