// RACS - Remote Audio Caching Server
// Copyright (c) 2025 RACS Labs, LLC. All rights reserved.
//
// Licensed under the RACS Source Available License (RACS-SAL-1.0).
// Non-commercial use only. Commercial use requires a paid license.
// Contact: sales@racslabs.com
//

#ifndef RACS_SST_H
#define RACS_SST_H


#ifdef __cplusplus
extern "C" {
#endif


#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include "types.h"


typedef struct __attribute__ ((packed)) {
    racs_uint64 key[3]; // { stream-id, timestamp, version }
    racs_uint32 offset;
    racs_uint32 block_size;
    racs_uint32 checksum;
} racs_sst_index_entry;

typedef struct {
    size_t size;
    racs_uint8 *data; // mmap pointer
    racs_uint8 *index_ptr;
    racs_uint16 num_entries;
} racs_sst;


racs_sst *racs_sst_open(const char *path);

void racs_sst_destroy(racs_sst *sst);

racs_sst_index_entry *racs_sst_get_index(racs_sst *sst);


#ifdef __cplusplus
}
#endif

#endif //RACS_SST_H
