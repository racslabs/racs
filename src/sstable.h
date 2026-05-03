// RACS - Remote Audio Caching Server
// Copyright (c) 2025 RACS Labs, LLC. All rights reserved.
//
// Licensed under the RACS Source Available License (RACS-SAL-1.0).
// Non-commercial use only. Commercial use requires a paid license.
// Contact: sales@racslabs.com
//

#ifndef RACS_SSTABLE_H
#define RACS_SSTABLE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include "types.h"

typedef struct __attribute__((packed)) {
    racs_uint64 key[3];      // { stream-id, timestamp, version }
    racs_uint32 offset;
    racs_uint32 block_size;
    racs_uint32 checksum;
} racs_sstable_index_entry;

typedef struct {
    size_t      size;
    racs_uint8 *data;      // mmap pointer
    racs_uint8 *index_ptr;
    racs_uint16 num_entries;
} racs_sstable;


racs_sstable *racs_sstable_open(const char *path);

void racs_sstable_destroy(racs_sstable *sst);

racs_sstable_index_entry *racs_sstable_get_index(racs_sstable *sst);

#ifdef __cplusplus
}
#endif

#endif //RACS_SSTABLE_H
