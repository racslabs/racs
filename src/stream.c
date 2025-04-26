#include "stream.h"

const char* const rats_stream_status_string[] = {
        "",
        "Malformed atsp frame.",
        "Stream is closed or currently in use.",
        "Invalid sample rate.",
        "Invalid channels.",
        "Invalid bit depth."
};

int rats_streamcreate(rats_cache* mcache, rats_uint64 stream_id, rats_uint32 sample_rate, rats_uint16 channels) {
    rats_streaminfo streaminfo;
    memset(&streaminfo, 0, sizeof(rats_streaminfo));

    streaminfo.sample_rate = sample_rate;
    streaminfo.channels = channels;
    streaminfo.bit_depth = 16;

    return rats_streaminfo_put(mcache, &streaminfo, stream_id);
}

int rats_streamappend(rats_cache* mcache, rats_multi_memtable* mmt, rats_streamkv* kv, rats_uint8* data) {
    rats_sp frame;
    if (!rats_sp_parse(data, &frame))
        return RATS_STREAM_MALFORMED;

    char* mac_addr = rats_streamkv_get(kv, frame.header.stream_id);
    if (!mac_addr || !rats_mac_addr_cmp(frame.header.mac_addr, mac_addr))
        return RATS_STREAM_CONFLICT;

    rats_streamkv_put(kv, frame.header.stream_id, frame.header.mac_addr);

    rats_streaminfo streaminfo;
    memset(&streaminfo, 0, sizeof(rats_streaminfo));
    rats_streaminfo_get(mcache, &streaminfo, frame.header.stream_id);

    if (frame.header.sample_rate != streaminfo.sample_rate)
        return RATS_STREAM_INVALID_SAMPLE_RATE;

    if (frame.header.channels != streaminfo.channels)
        return RATS_STREAM_INVALID_CHANNELS;

    if (frame.header.bit_depth != 16)
        return RATS_STREAM_INVALID_BITDEPTH;

    if (streaminfo.ref == 0 && streaminfo.size == 0)
        streaminfo.ref = rats_time_now();

    rats_time offset = rats_streaminfo_offset(&streaminfo);
    rats_uint64 key[2] = {frame.header.stream_id, offset};
    rats_multi_memtable_append(mmt, key, frame.pcm_block, frame.header.block_size);

    streaminfo.size += frame.header.block_size;
    rats_streaminfo_put(mcache, &streaminfo, frame.header.stream_id);

    return RATS_STREAM_OK;
}

int rats_streamopen(rats_streamkv* kv, rats_uint64 stream_id) {
    char* mac_addr = rats_streamkv_get(kv, stream_id);
    if (mac_addr) {
        perror("Stream is already open");
        return 0;
    }

    char _mac_addr[6];
    memset(_mac_addr, 0, 6);

    rats_streamkv_put(kv, stream_id, _mac_addr);
    return 1;
}

int rats_streamclose(rats_streamkv* kv, rats_uint64 stream_id) {
    char* mac_addr = rats_streamkv_get(kv, stream_id);
    if (!mac_addr) {
        perror("Stream is not open");
        return 0;
    }

    rats_streamkv_delete(kv, stream_id);
    return 1;
}

rats_streamkv* rats_streamkv_create(int capacity) {
    rats_streamkv* kv = malloc(sizeof(rats_streamkv));
    if (!kv) {
        perror("Failed to allocate rats_streamkv");
        return NULL;
    }

    kv->kv = rats_kvstore_create(capacity, rats_streamkv_hash, rats_streamkv_cmp, rats_streamkv_destroy_entry);
    pthread_rwlock_init(&kv->rwlock, NULL);

    return kv;
}

char* rats_streamkv_get(rats_streamkv* kv, rats_uint64 stream_id) {
    pthread_rwlock_rdlock(&kv->rwlock);

    rats_uint64 key[2] = {stream_id, 0};
    char* mac_addr = rats_kvstore_get(kv->kv, key);

    pthread_rwlock_unlock(&kv->rwlock);

    return mac_addr;
}

void rats_streamkv_put(rats_streamkv* kv, rats_uint64 stream_id, char mac_addr[]) {
    pthread_rwlock_wrlock(&kv->rwlock);

    rats_uint64* key = malloc(2 * sizeof(rats_uint64));
    if (!key) {
        perror("Failed to allocate key.");
        return;
    }

    key[0] = stream_id;
    key[1] = 0;

    char* _mac_addr = malloc(6);
    if (!_mac_addr) {
        perror("Failed to allocate mac_addr buffer");
        return;
    }

    memcpy(_mac_addr, mac_addr, 6);

    rats_kvstore_put(kv->kv, key, _mac_addr);
    pthread_rwlock_unlock(&kv->rwlock);
}

void rats_streamkv_delete(rats_streamkv* kv, rats_uint64 stream_id) {
    pthread_rwlock_wrlock(&kv->rwlock);

    rats_uint64 key[2] = {stream_id, 0};
    rats_kvstore_delete(kv->kv, key);

    pthread_rwlock_unlock(&kv->rwlock);
}

void rats_streamkv_destroy(rats_streamkv* kv) {
    pthread_rwlock_wrlock(&kv->rwlock);
    rats_kvstore_destroy(kv->kv);
    pthread_rwlock_unlock(&kv->rwlock);
    pthread_rwlock_destroy(&kv->rwlock);
}

uint64_t rats_streamkv_hash(void* key) {
    uint64_t hash[2];
    murmur3_x64_128(key, 2 * sizeof(uint64_t), 0, hash);
    return hash[0];
}

int rats_streamkv_cmp(void* a, void* b) {
    uint64_t* x = (uint64_t*)a;
    uint64_t* y = (uint64_t*)b;
    return x[0] == y[0] && x[1] == y[1];
}

void rats_streamkv_destroy_entry(void* key, void* value) {
    free(key);
    free(value);
}

int rats_mac_addr_cmp(const char* src, const char* dest) {
    char _mac_addr[6] = {0};
    if (memcmp(dest, _mac_addr, 6) == 0) return 1;
    if (memcmp(src, dest, 6) == 0) return 1;
    return 0;
}