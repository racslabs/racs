
#ifndef RACS_STREAM_H
#define RACS_STREAM_H

#include "streaminfo.h"
#include "memtable.h"
#include "frame.h"
#include "result.h"
#include "log.h"

typedef enum {
    RACS_STREAM_OK,
    RACS_STREAM_MALFORMED,
    RACS_STREAM_CONFLICT,
    RACS_STREAM_INVALID_SAMPLE_RATE,
    RACS_STREAM_INVALID_CHANNELS,
    RACS_STREAM_INVALID_BITDEPTH,
    RACS_STREAM_NOT_FOUND
} racs_stream_status;

typedef struct {
    racs_kvstore *kv;
    pthread_rwlock_t rwlock;
} racs_streamkv;

extern const char *const racs_stream_status_string[];

int racs_streamcreate(racs_cache *mcache, const char* stream_id, racs_uint32 sample_rate, racs_uint16 channels);

int racs_streamappend(racs_cache *mcache, racs_multi_memtable *mmt, racs_streamkv *kv, racs_uint8 *data);

int racs_streamopen(racs_streamkv *kv, racs_uint64 stream_id);

int racs_streamclose(racs_streamkv *kv, racs_uint64 stream_id);

racs_uint64 *racs_streamkv_get(racs_streamkv *kv, racs_uint64 stream_id);

void racs_streamkv_delete(racs_streamkv *kv, racs_uint64 stream_id);

void racs_streamkv_put(racs_streamkv *kv, racs_uint64 stream_id, racs_uint64 session_id[]);

racs_streamkv *racs_streamkv_create(int capacity);

void racs_streamkv_destroy(racs_streamkv *kv);

racs_uint64 racs_streamkv_hash(void *key);

int racs_streamkv_cmp(void *a, void *b);

void racs_streamkv_destroy_entry(void *key, void *value);

int racs_session_cmp(const racs_uint64 *src, const racs_uint64 *dest);

#endif //RACS_STREAM_H
