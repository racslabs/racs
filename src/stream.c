#include "stream.h"

int auxts_stream(auxts_cache* mcache, auxts_uint64 stream_id, auxts_uint32 sample_rate, auxts_uint16 channels) {
    auxts_streaminfo streaminfo;
    streaminfo.sample_rate = sample_rate;
    streaminfo.channels = channels;
    streaminfo.bit_depth = 16;
    streaminfo.ref = 0;
    streaminfo.size = 0;

    return auxts_streaminfo_put(mcache, &streaminfo, stream_id);
}

auxts_streamkv* auxts_streamkv_create(int capacity) {
    auxts_streamkv* kv = malloc(sizeof(auxts_streamkv));
    if (!kv) {
        perror("Failed to allocate auxts_streamkv");
        return NULL;
    }

    kv->kv = auxts_kvstore_create(capacity, auxts_streamkv_hash, auxts_streamkv_cmp, auxts_streamkv_destroy_entry);
    pthread_rwlock_init(&kv->rwlock, NULL);

    return kv;
}

void auxts_streamkv_destroy(auxts_streamkv* kv) {
    pthread_rwlock_destroy(&kv->rwlock);
    auxts_kvstore_destroy(kv->kv);
}

uint64_t auxts_streamkv_hash(void* key) {
    uint64_t hash[2];
    murmur3_x64_128(key, 2 * sizeof(uint64_t), 0, hash);
    return hash[0];
}

int auxts_streamkv_cmp(void* a, void* b) {
    uint64_t* x = (uint64_t*)a;
    uint64_t* y = (uint64_t*)b;
    return x[0] == y[0] && x[1] == y[1];
}

void auxts_streamkv_destroy_entry(void* key, void* value) {
    free(value);
}

