
#ifndef AUXTS_STREAM_H
#define AUXTS_STREAM_H

#include "streaminfo.h"
#include "memtable.h"
#include "sp.h"
#include "result.h"

typedef enum {
    AUXTS_STREAM_OK,
    AUXTS_STREAM_MALFORMED,
    AUXTS_STREAM_CONFLICT,
    AUXTS_STREAM_INVALID_SAMPLE_RATE,
    AUXTS_STREAM_INVALID_CHANNELS,
    AUXTS_STREAM_INVALID_BITDEPTH
} rats_stream_status;

typedef struct {
    rats_kvstore* kv;
    pthread_rwlock_t rwlock;
} rats_streamkv;

extern const char* const rats_stream_status_string[];

int rats_streamcreate(rats_cache* mcache, rats_uint64 stream_id, rats_uint32 sample_rate, rats_uint16 channels);
int rats_streamappend(rats_cache* mcache, rats_multi_memtable* mmt, rats_streamkv* kv, uint8_t* data);
int rats_streamopen(rats_streamkv* kv, rats_uint64 stream_id);
int rats_streamclose(rats_streamkv* kv, rats_uint64 stream_id);
char* rats_streamkv_get(rats_streamkv* kv, rats_uint64 stream_id);
void rats_streamkv_delete(rats_streamkv* kv, rats_uint64 stream_id);
void rats_streamkv_put(rats_streamkv* kv, rats_uint64 stream_id, char mac_addr[]);
rats_streamkv* rats_streamkv_create(int capacity);
void rats_streamkv_destroy(rats_streamkv* kv);
uint64_t rats_streamkv_hash(void* key);
int rats_streamkv_cmp(void* a, void* b);
void rats_streamkv_destroy_entry(void* key, void* value);
int rats_mac_addr_cmp(const char* src, const char* dest);

#endif //AUXTS_STREAM_H
