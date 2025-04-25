
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
} auxts_stream_status;

typedef struct {
    auxts_kvstore* kv;
    pthread_rwlock_t rwlock;
} auxts_streamkv;

extern const char* const auxts_stream_status_string[];

int auxts_streamcreate(auxts_cache* mcache, auxts_uint64 stream_id, auxts_uint32 sample_rate, auxts_uint16 channels);
int auxts_streamappend(auxts_cache* mcache, auxts_multi_memtable* mmt, auxts_streamkv* kv, uint8_t* data);
int auxts_streamopen(auxts_streamkv* kv, auxts_uint64 stream_id);
int auxts_streamclose(auxts_streamkv* kv, auxts_uint64 stream_id);
char* auxts_streamkv_get(auxts_streamkv* kv, auxts_uint64 stream_id);
void auxts_streamkv_delete(auxts_streamkv* kv, auxts_uint64 stream_id);
void auxts_streamkv_put(auxts_streamkv* kv, auxts_uint64 stream_id, char mac_addr[]);
auxts_streamkv* auxts_streamkv_create(int capacity);
void auxts_streamkv_destroy(auxts_streamkv* kv);
uint64_t auxts_streamkv_hash(void* key);
int auxts_streamkv_cmp(void* a, void* b);
void auxts_streamkv_destroy_entry(void* key, void* value);
int auxts_mac_addr_cmp(const char* src, const char* dest);

#endif //AUXTS_STREAM_H
