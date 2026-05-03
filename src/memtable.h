
#ifndef RACS_MEMTABLE_H
#define RACS_MEMTABLE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "types.h"
#include "sstable.h"
#include <stdio.h>
#include <limits.h>
#include <pthread.h>

typedef struct {
    racs_uint64 key[3];
    racs_uint64 lsn;
    racs_uint32 block_size;
    racs_uint32 checksum;
    racs_uint8 *block;
} racs_memtable_entry;

typedef struct racs_memtable {
    racs_memtable_entry *entries;
    racs_uint16 num_entries;
    racs_uint16 capacity;
    pthread_mutex_t mutex;
    struct racs_memtable *next;
    struct racs_memtable *prev;
} racs_memtable;


racs_uint8 *racs_memtable_to_sstable(racs_memtable *mt, size_t *sst_size);

racs_memtable *racs_memtable_create(int capacity);

void racs_memtable_append(racs_memtable *mt,
                          const racs_uint64 *key,
                          const racs_uint8 *block,
                          racs_uint16 block_size,
                          racs_uint32 checksum,
                          racs_uint64 lsn);

#ifdef __cplusplus
}
#endif

#endif //RACS_MEMTABLE_H
