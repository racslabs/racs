#include "stream.h"

const char* const auxts_stream_status_string[] = {
        "",
        "Malformed atsp frame.",
        "Stream is closed or currently in use.",
        "Invalid sample rate.",
        "Invalid channels.",
        "Invalid bit depth."
};

int auxts_streamcreate(auxts_cache* mcache, auxts_uint64 stream_id, auxts_uint32 sample_rate, auxts_uint16 channels) {
    auxts_streaminfo streaminfo;
    memset(&streaminfo, 0, sizeof(auxts_streaminfo));

    streaminfo.sample_rate = sample_rate;
    streaminfo.channels = channels;
    streaminfo.bit_depth = 16;

    return auxts_streaminfo_put(mcache, &streaminfo, stream_id);
}

int auxts_streamappend(auxts_cache* mcache, auxts_multi_memtable* mmt, auxts_streamkv* kv, uint8_t* data) {
    rats_sp frame;
    if (!rats_sp_parse(data, &frame))
        return AUXTS_STREAM_MALFORMED;

    char* mac_addr = auxts_streamkv_get(kv, frame.header.stream_id);
    if (!mac_addr || !auxts_mac_addr_cmp(frame.header.mac_addr, mac_addr))
        return AUXTS_STREAM_CONFLICT;

    auxts_streamkv_put(kv, frame.header.stream_id, frame.header.mac_addr);

    auxts_streaminfo streaminfo;
    memset(&streaminfo, 0, sizeof(auxts_streaminfo));
    auxts_streaminfo_get(mcache, &streaminfo, frame.header.stream_id);

    if (frame.header.sample_rate != streaminfo.sample_rate)
        return AUXTS_STREAM_INVALID_SAMPLE_RATE;

    if (frame.header.channels != streaminfo.channels)
        return AUXTS_STREAM_INVALID_CHANNELS;

    if (frame.header.bit_depth != 16)
        return AUXTS_STREAM_INVALID_BITDEPTH;

    if (streaminfo.ref == 0 && streaminfo.size == 0)
        streaminfo.ref = auxts_time_now();

    auxts_time offset = auxts_streaminfo_offset(&streaminfo);
    auxts_uint64 key[2] = {frame.header.stream_id, offset};
    auxts_multi_memtable_append(mmt, key, frame.pcm_block, frame.header.block_size);

    streaminfo.size += frame.header.block_size;
    auxts_streaminfo_put(mcache, &streaminfo, frame.header.stream_id);

    return AUXTS_STREAM_OK;
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

    auxts_uint64* key = malloc(2 * sizeof(auxts_uint64));
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
    free(key);
    free(value);
}

int auxts_mac_addr_cmp(const char* src, const char* dest) {
    char _mac_addr[6] = {0};
    if (memcmp(dest, _mac_addr, 6) == 0) return 1;
    if (memcmp(src, dest, 6) == 0) return 1;
    return 0;
}