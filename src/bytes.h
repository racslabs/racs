// RACS - Remote Audio Caching Server
// Copyright (c) 2025 RACS Labs, LLC. All rights reserved.
//
// Licensed under the RACS Source Available License (RSAL-1.0).
// Non-commercial use only. Commercial use requires a paid license.
// Contact: sales@racslabs.com
//
// SPDX-License-Identifier: RACS-SAL-1.0

#ifndef RACS_BYTES_H
#define RACS_BYTES_H

#include "export.h"
#include "types.h"
#include <string.h>
#include <unistd.h>
#include <stdint.h>

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

#endif //RACS_BYTES_H
