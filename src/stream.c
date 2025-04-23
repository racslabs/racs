#include "stream.h"

int auxts_stream(auxts_cache* mcache, auxts_uint64 stream_id, auxts_uint32 sample_rate, auxts_uint16 channels) {
    auxts_streaminfo streaminfo;
    memset(&streaminfo, 0, sizeof(auxts_streaminfo));

    streaminfo.sample_rate = sample_rate;
    streaminfo.channels = channels;
    streaminfo.bit_depth = 16;

    return auxts_streaminfo_put(mcache, &streaminfo, stream_id);
}

int auxts_streamappend(auxts_cache* mcache, auxts_multi_memtable* mmt, auxts_streamkv* kv, uint8_t* data) {
    auxts_atsp_frame frame;
    if (!auxts_atsp_frame_read(data, &frame)) return 0;

    char* mac_addr = auxts_streamkv_get(kv, frame.header.stream_id);
    if (!mac_addr) return 0;
    if (!auxts_mac_addr_cmp(mac_addr, frame.header.mac_addr)) return 0;

    auxts_streaminfo streaminfo;
    auxts_streaminfo_get(mcache, &streaminfo, frame.header.stream_id);

    if (frame.header.sample_rate != streaminfo.sample_rate) return 0;
    if (frame.header.channels != streaminfo.channels) return 0;
    if (frame.header.bit_depth != 16) return 0;

    if (streaminfo.ref == 0 && streaminfo.size == 0) streaminfo.ref = auxts_time_now();

    auxts_time offset = auxts_streaminfo_offset(&streaminfo);
    auxts_uint64 key[2] = {frame.header.stream_id, offset};
    auxts_multi_memtable_append(mmt, key, frame.pcm_block, frame.header.block_size);

    streaminfo.size += frame.header.block_size;
    auxts_streaminfo_put(mcache, &streaminfo, frame.header.stream_id);

    return 1;
}

int auxts_streamopen(auxts_streamkv* kv, auxts_uint64 stream_id) {
    char* mac_addr = auxts_streamkv_get(kv, stream_id);
    if (mac_addr) {
        perror("Stream is already open");
        return 0;
    }

    char _mac_addr[6];
    memset(_mac_addr, 0, 6);

    auxts_streamkv_put(kv, stream_id, _mac_addr);
    return 1;
}

int auxts_streamclose(auxts_streamkv* kv, auxts_uint64 stream_id) {
    char* mac_addr = auxts_streamkv_get(kv, stream_id);
    if (!mac_addr) {
        perror("Stream is not open");
        return 0;
    }

    auxts_streamkv_delete(kv, stream_id);
    return 1;
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

char* auxts_streamkv_get(auxts_streamkv* kv, auxts_uint64 stream_id) {
    pthread_rwlock_rdlock(&kv->rwlock);

    auxts_uint64 key[2] = {stream_id, 0};
    char* mac_addr = auxts_kvstore_get(kv->kv, key);

    pthread_rwlock_unlock(&kv->rwlock);

    return mac_addr;
}

void auxts_streamkv_put(auxts_streamkv* kv, auxts_uint64 stream_id, char mac_addr[]) {
    pthread_rwlock_wrlock(&kv->rwlock);

    auxts_uint64 key[2] = {stream_id, 0};

    char* _mac_addr = malloc(6);
    if (!_mac_addr) {
        perror("Failed to allocate mac_addr buffer");
        return;
    }

    auxts_kvstore_put(kv->kv, key, _mac_addr);
    pthread_rwlock_unlock(&kv->rwlock);
}

void auxts_streamkv_delete(auxts_streamkv* kv, auxts_uint64 stream_id) {
    pthread_rwlock_wrlock(&kv->rwlock);

    auxts_uint64 key[2] = {stream_id, 0};
    auxts_kvstore_delete(kv->kv, key);

    pthread_rwlock_unlock(&kv->rwlock);
}

void auxts_streamkv_destroy(auxts_streamkv* kv) {
    pthread_rwlock_wrlock(&kv->rwlock);
    auxts_kvstore_destroy(kv->kv);
    pthread_rwlock_unlock(&kv->rwlock);
    pthread_rwlock_destroy(&kv->rwlock);
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

int auxts_mac_addr_cmp(const char* a, const char* b) {
    return a[0] == b[0] ||
            a[1] == b[1] ||
            a[2] == b[2] ||
            a[3] == b[3] ||
            a[4] == b[4] ||
            a[5] == b[5];
}