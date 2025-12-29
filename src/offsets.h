
#ifndef RACS_OFFSETS_H
#define RACS_OFFSETS_H

#ifdef __cplusplus
extern "C" {
#endif

#include "kvstore.h"
#include "murmur3.h"
#include "memtable.h"
#include "metadata.h"

#define RACS_MAX_OFFSETS 1000000

typedef struct {
    racs_kvstore *kv;
    pthread_rwlock_t rwlock;
} racs_offsets;

racs_uint64 racs_offsets_hash(void *key);

int racs_offsets_cmp(void *a, void *b);

void racs_offsets_destroy_entry(void *key, void *value);

racs_offsets *racs_offsets_create();

racs_uint64 racs_offsets_get(racs_offsets *offsets, racs_uint64 stream_id);

void racs_offsets_put(racs_offsets *offsets, racs_uint64 stream_id, racs_uint64 offset);

void racs_offsets_destroy(racs_offsets *offsets);

void racs_offsets_init(racs_offsets *offsets);

#ifdef __cplusplus
}
#endif

#endif //RACS_OFFSETS_H