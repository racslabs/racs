// RACS - Remote Audio Caching Server
// Copyright (c) 2025 RACS Labs, LLC. All rights reserved.
//
// Licensed under the RACS Source Available License (RACS-SAL-1.0).
// Non-commercial use only. Commercial use requires a paid license.
// Contact: sales@racslabs.com
//
// SPDX-License-Identifier: RACS-SAL-1.0

#include "stream.h"

const char *const racs_stream_status_string[] = {
        "",
        "Malformed rsp frame.",
        "Stream is closed or currently in use.",
        "Invalid sample rate.",
        "Invalid channels.",
        "Invalid bit depth.",
        "Stream not found."
};

int racs_streamcreate(racs_cache *mcache, const char* stream_id, racs_uint32 sample_rate, racs_uint16 channels, racs_uint16 bit_depth, racs_time ref) {
    racs_streaminfo streaminfo;
    memset(&streaminfo, 0, sizeof(racs_streaminfo));

    streaminfo.sample_rate = sample_rate;
    streaminfo.channels = channels;
    streaminfo.bit_depth = bit_depth;
    streaminfo.id_size = strlen(stream_id) + 1;
    streaminfo.id = (char*)stream_id;
    streaminfo.ttl = -1;
    streaminfo.ref = ref;

    racs_uint64 hash = racs_hash(stream_id);
    if (racs_streaminfo_get(mcache, &streaminfo, hash)) return 0;

    size_t size = racs_streaminfo_size(&streaminfo);
    racs_uint8 *data = malloc(size);
    if (!data) return 0;

    racs_streaminfo_write(data, &streaminfo);
    racs_streaminfo_flush(data, size, hash);
    racs_streaminfo_put(mcache, &streaminfo, hash);

    return 1;
}

int racs_streamappend(racs_cache *mcache, racs_multi_memtable *mmt, racs_streamkv *kv, racs_uint8 *data) {
    racs_frame frame;
    if (!racs_frame_parse(data, &frame))
        return RACS_STREAM_MALFORMED;

    racs_uint8 *session_id = racs_streamkv_get(kv, frame.header.stream_id);
    if (!session_id) return RACS_STREAM_CONFLICT;

    if (!racs_session_cmp(frame.header.session_id, session_id))
        return RACS_STREAM_CONFLICT;

    racs_streaminfo streaminfo;
    memset(&streaminfo, 0, sizeof(racs_streaminfo));
    int rc = racs_streaminfo_get(mcache, &streaminfo, frame.header.stream_id);

    if (rc == 0) return RACS_STREAM_NOT_FOUND;
    racs_streamkv_put(kv, frame.header.stream_id, frame.header.session_id);

    racs_time offset = racs_streaminfo_offset(&streaminfo);
    racs_uint64 key[2] = {frame.header.stream_id, offset};

    racs_wal_append(RACS_OP_CODE_APPEND, 34 + frame.header.block_size, data);
    racs_multi_memtable_append(mmt, key, frame.pcm_block, frame.header.block_size, frame.header.checksum);

    streaminfo.size += frame.header.block_size;

    racs_wal_append(RACS_OP_CODE_CREATE, 36 + streaminfo.id_size, (racs_uint8 *)&streaminfo);
    racs_streaminfo_put(mcache, &streaminfo, frame.header.stream_id);

    return RACS_STREAM_OK;
}

int racs_streamopen(racs_streamkv *kv, racs_uint64 stream_id) {
    racs_uint8 *session_id = racs_streamkv_get(kv, stream_id);
    if (session_id) {
        racs_log_error("Stream is already open");
        return 0;
    }

    racs_uint8 _session_id[16];
    memset(_session_id, 0, 16);

    racs_streamkv_put(kv, stream_id, _session_id);
    return 1;
}

int racs_streamclose(racs_streamkv *kv, racs_uint64 stream_id) {
    racs_uint8 *session_id = racs_streamkv_get(kv, stream_id);
    if (!session_id) {
        racs_log_info("Stream is not open");
        return 0;
    }

    racs_streamkv_delete(kv, stream_id);
    return 1;
}

racs_streamkv *racs_streamkv_create(int capacity) {
    racs_streamkv *kv = malloc(sizeof(racs_streamkv));
    if (!kv) {
        racs_log_error("Failed to allocate racs_streamkv");
        return NULL;
    }

    kv->kv = racs_kvstore_create(capacity, racs_streamkv_hash, racs_streamkv_cmp, racs_streamkv_destroy_entry);
    pthread_rwlock_init(&kv->rwlock, NULL);

    return kv;
}

racs_uint8 *racs_streamkv_get(racs_streamkv *kv, racs_uint64 stream_id) {
    pthread_rwlock_rdlock(&kv->rwlock);

    racs_uint64 key[2] = {stream_id, 0};
    racs_uint8 *session_id = racs_kvstore_get(kv->kv, key);

    pthread_rwlock_unlock(&kv->rwlock);

    return session_id;
}

void racs_streamkv_put(racs_streamkv *kv, racs_uint64 stream_id, racs_uint8 *session_id) {
    pthread_rwlock_wrlock(&kv->rwlock);

    racs_uint64 *key = malloc(2 * sizeof(racs_uint64));
    if (!key) {
        racs_log_error("Failed to allocate key.");
        return;
    }

    key[0] = stream_id;
    key[1] = 0;

    racs_uint8 *_session_id = malloc(16);
    if (!_session_id) {
        racs_log_error("Failed to allocate session_id");
        return;
    }

    memcpy(_session_id, session_id, 16);

    racs_kvstore_put(kv->kv, key, _session_id);
    pthread_rwlock_unlock(&kv->rwlock);
}

void racs_streamkv_delete(racs_streamkv *kv, racs_uint64 stream_id) {
    pthread_rwlock_wrlock(&kv->rwlock);

    racs_uint64 key[2] = {stream_id, 0};
    racs_kvstore_delete(kv->kv, key);

    pthread_rwlock_unlock(&kv->rwlock);
}

void racs_streamkv_destroy(racs_streamkv *kv) {
    pthread_rwlock_wrlock(&kv->rwlock);
    racs_kvstore_destroy(kv->kv);
    pthread_rwlock_unlock(&kv->rwlock);
    pthread_rwlock_destroy(&kv->rwlock);
}

racs_uint64 racs_streamkv_hash(void *key) {
    racs_uint64 hash[2];
    murmur3_x64_128(key, 2 * sizeof(racs_uint64), 0, hash);
    return hash[0];
}

int racs_streamkv_cmp(void *a, void *b) {
    racs_uint64 *x = (racs_uint64 *) a;
    racs_uint64 *y = (racs_uint64 *) b;
    return x[0] == y[0];
}

void racs_streamkv_destroy_entry(void *key, void *value) {
    free(key);
    free(value);
}

int racs_session_cmp(const racs_uint8 *src, const racs_uint8 *dest) {
    racs_uint8 _session_id[16];
    memset(_session_id, 0, 16);

    if (memcmp(dest, _session_id, 16) == 0) return 1;
    if (memcmp(src, dest, 16) == 0) return 1;
    return 0;
}
