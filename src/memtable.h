// RACS - Remote Audio Caching Server
// Copyright (c) 2025 RACS Labs, LLC. All rights reserved.
//
// Licensed under the RACS Source Available License (RSAL-1.0).
// Non-commercial use only. Commercial use requires a paid license.
// Contact: sales@racslabs.com
//
// SPDX-License-Identifier: RACS-SAL-1.0

#ifndef RACS_MEMTABLE_H
#define RACS_MEMTABLE_H

#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <errno.h>
#include <pthread.h>
#include "bytes.h"
#include "time.h"
#include "murmur3.h"
#include "crc32c.h"
#include "version.h"

#ifdef __cplusplus
extern "C" {
#endif

#define RACS_BLOCK_ALIGN 4096

#define RACS_MAX_BLOCK_SIZE 65536

#define RACS_INDEX_ENTRY_SIZE 24

#define RACS_HEADER_SIZE 16

#define RACS_MEMTABLE_ENTRY_METADATA_SIZE 22

#define RACS_TRAILER_SIZE 2

typedef struct {
    racs_uint64 key[2];
    racs_uint32 checksum;
    racs_uint16 block_size;
    racs_uint8 *block;
} racs_memtable_entry;

typedef struct {
    racs_memtable_entry *entries;
    racs_uint16 num_entries;
    racs_uint16 capacity;
    pthread_mutex_t mutex;
    struct racs_memtable *next;
    struct racs_memtable *prev;
} racs_memtable;

typedef struct {
    racs_uint64 key[2];
    size_t offset;
} racs_sstable_index_entry;

typedef struct {
    int fd;
    size_t size;
    racs_uint8 *data;
    racs_uint16 num_entries;
    racs_sstable_index_entry *index_entries;
} racs_sstable;

typedef struct {
    int index;
    int num_tables;
    racs_memtable *head;
    racs_memtable *tail;
    pthread_mutex_t mutex;
} racs_multi_memtable;

racs_multi_memtable *racs_multi_memtable_create(int num_tables, int capacity);

void racs_multi_memtable_append(racs_multi_memtable *mmt, racs_uint64 *key, racs_uint8 *block, racs_uint16 block_size, racs_uint32 checksum);

void racs_multi_memtable_destroy(racs_multi_memtable *mmt);

void racs_multi_memtable_flush(racs_multi_memtable *mmt);

void racs_multi_memtable_move_to_head(racs_multi_memtable *mmt, racs_memtable *mt);

racs_sstable *racs_sstable_read(const char *filename);

racs_sstable *racs_sstable_read_in_memory(racs_uint8 *data, size_t size);

void racs_sstable_destroy_except_data(racs_sstable *sst);

racs_memtable_entry *racs_memtable_entry_read(racs_uint8 *buf, size_t offset);

void racs_memtable_append(racs_memtable *mt, racs_uint64 *key, racs_uint8 *block, racs_uint16 block_size, racs_uint32 checksum);

void racs_memtable_flush(racs_memtable *mt);

void racs_memtable_flush_async(racs_memtable *mt);

void racs_memtable_destroy(racs_memtable *mt);

void racs_sstable_read_index_entries(racs_sstable *sst);

void racs_memtable_write(racs_memtable *mt);

void racs_sstable_read_index_entries_in_memory(racs_sstable *sst, racs_uint8 *data);

void racs_sstable_write(racs_uint8 *buf, racs_sstable *sst, size_t offset);

void
racs_sstable_index_entry_update(racs_sstable_index_entry *index_entry, racs_memtable_entry *mt_entry, off_t offset);

void racs_sstable_path(racs_int64 timestamp, char **path);

racs_sstable *racs_sstable_create(int num_entries);

racs_memtable *racs_memtable_create(int capacity);

off_t racs_memtable_to_sstable(racs_uint8 *buf, racs_sstable *sst, racs_memtable *mt);

off_t racs_memtable_entry_write(racs_uint8 *buf, const racs_memtable_entry *mt_entry, off_t offset);

off_t racs_sstable_index_entries_write(racs_uint8 *buf, racs_sstable *sst, off_t offset);

off_t racs_write_index_entry(racs_uint8 *buf, racs_sstable_index_entry *index_entry, off_t offset);

racs_uint8 *racs_allocate_buffer(size_t size, racs_sstable *sst);

int racs_sstable_open(const char *path, racs_sstable *sst);

#ifdef __cplusplus
}
#endif

#endif //RACS_MEMTABLE_H
