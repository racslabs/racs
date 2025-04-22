
#ifndef AUXTS_STREAM_H
#define AUXTS_STREAM_H

#include "streaminfo.h"

typedef struct {
    auxts_kvstore* kv;
    pthread_rwlock_t rwlock;
} auxts_streamkv;

int auxts_stream(auxts_cache* mcache, auxts_uint64 stream_id, auxts_uint32 sample_rate, auxts_uint16 channels);

char* auxts_streamkv_get(auxts_uint64 stream_id);
auxts_streamkv* auxts_streamkv_create(int capacity);
void auxts_streamkv_destroy(auxts_streamkv* kv);
uint64_t auxts_streamkv_hash(void* key);
int auxts_streamkv_cmp(void* a, void* b);
void auxts_streamkv_destroy_entry(void* key, void* value);

#endif //AUXTS_STREAM_H
