// RACS - Remote Audio Caching Server
// Copyright (c) 2025 RACS Labs, LLC. All rights reserved.
//
// Licensed under the RACS Source Available License (RACS-SAL-1.0).
// Non-commercial use only. Commercial use requires a paid license.
// Contact: sales@racslabs.com
//
// SPDX-License-Identifier: RACS-SAL-1.0

#include "stream.h"
#include "pack.h"

const char *const racs_stream_status_string[] = {
        "",
        "Malformed rsp frame.",
        "Stream is closed or currently in use.",
        "Invalid sample rate.",
        "Invalid channels.",
        "Invalid bit depth.",
        "Stream not found."
};

int racs_stream_create(racs_versions *versions, const char* stream_id, racs_uint32 sample_rate, racs_uint16 channels, racs_uint16 bit_depth) {
    racs_metadata metadata;
    racs_uint64 hash = racs_hash(stream_id);

    if (racs_metadata_get(&metadata, hash)) {
        racs_metadata_destroy(&metadata);
        return 0;
    }

    metadata.sample_rate = sample_rate;
    metadata.channels = channels;
    metadata.bit_depth = bit_depth;
    metadata.id_size = strlen(stream_id) + 1;
    metadata.id = (char *)stream_id;
    metadata.ttl = -1;
    metadata.ref = racs_time_now();
    metadata.version = racs_versions_get(versions, hash);

    size_t size = racs_metadata_size(&metadata);
    racs_uint8 *data = malloc(size);
    if (!data) return 0;

    racs_metadata_write(data, &metadata);
    racs_wal_append(RACS_OP_CODE_CREATE, size, data);
    free(data);

    racs_metadata_put(&metadata, hash);
    racs_metadata_destroy(&metadata);

    return 1;
}

void racs_stream_batch_append(racs_multi_memtable *mmt, racs_offsets *offsets, racs_versions *versions, racs_sessions *kv, racs_uint8 *data, size_t size) {
    msgpack_unpacked msg;
    msgpack_unpacked_init(&msg);

    if (msgpack_unpack_next(&msg, (char *)data, size, 0) == MSGPACK_UNPACK_PARSE_ERROR)
        perror("Error parsing response");

    size_t num_frames = msg.data.via.array.size;

    for (int i = 0; i < num_frames; ++i) {
        msgpack_object obj = msg.data.via.array.ptr[i];
        racs_uint8 *frame = racs_unpack_u8v(&obj);

        if (frame)
            racs_stream_append(mmt, offsets, versions, kv, frame);
    }
}

int racs_stream_append(racs_multi_memtable *mmt, racs_offsets *offsets, racs_versions *versions, racs_sessions *sessions, racs_uint8 *data) {
    racs_frame frame;
    if (!racs_frame_parse(data, &frame))
        return RACS_STREAM_MALFORMED;

    racs_uint8 *_session = racs_sessions_get(sessions, frame.header.stream_id);
    if (!_session) return RACS_STREAM_CONFLICT;

    racs_session session;
    racs_session_read(&session, _session);

    if (!racs_sessions_id_cmp(frame.header.session_id, session.id))
        return RACS_STREAM_CONFLICT;

    racs_uint64 offset = racs_offsets_get(offsets, frame.header.stream_id);
    racs_time timestamp = racs_offsets_timestamp(offset, session.ref, session.channels, session.bit_depth, session.sample_rate);
    racs_uint64 version = racs_versions_get(versions, frame.header.stream_id);
    racs_uint64 key[3] = { frame.header.stream_id, timestamp, version };

    racs_wal_append(RACS_OP_CODE_APPEND, 34 + frame.header.block_size, data);
    racs_multi_memtable_append(mmt, versions, key, frame.pcm_block, frame.header.block_size, frame.header.checksum, frame.header.flags);

    if (frame.header.flags == 1) {
        size_t decompressed_size;

        racs_uint8 *decompressed_block = racs_zstd_decompress(frame.pcm_block, frame.header.block_size, &decompressed_size);
        if (decompressed_block) {
            offset += decompressed_size;
            free(decompressed_block);
        }
    } else {
        offset += frame.header.block_size;
    }

    racs_offsets_put(offsets, frame.header.stream_id, offset);
    return RACS_STREAM_OK;
}

int racs_stream_open(racs_sessions *sessions, racs_uint64 stream_id, racs_uint8 *session_id) {
    racs_uint8 *_session = racs_sessions_get(sessions, stream_id);
    if (_session) {
        racs_log_error("Stream is already open");
        return 0;
    }

    racs_metadata metadata;
    if (!racs_metadata_get(&metadata, stream_id)) {
        racs_metadata_destroy(&metadata);
        racs_log_error("Stream does not exist");
        return 0;
    }

    racs_session session;
    memcpy(session.id, session_id, 16);
    session.bit_depth = metadata.bit_depth;
    session.channels = metadata.channels;
    session.sample_rate = metadata.sample_rate;
    session.stream_id = stream_id;
    session.ref = metadata.ref;

    _session = malloc(sizeof(racs_session));
    if (!_session) return 0;

    racs_session_write(_session, &session);
    racs_wal_append(RACS_OP_CODE_OPEN, sizeof(racs_session), _session);
    racs_sessions_put(sessions, stream_id, _session);

    return 1;
}

int racs_stream_close(racs_sessions *sessions, racs_uint64 stream_id) {
    racs_uint8 *session = racs_sessions_get(sessions, stream_id);
    if (!session) {
        racs_log_info("Stream is not open");
        return 0;
    }

    racs_wal_append(RACS_OP_CODE_CLOSE, sizeof(racs_uint64), (racs_uint8 *) &stream_id);
    racs_sessions_delete(sessions, stream_id);
    return 1;
}

racs_sessions *racs_sessions_create(int capacity) {
    racs_sessions *sessions = malloc(sizeof(racs_sessions));
    if (!sessions) {
        racs_log_error("Failed to allocate racs_sessions");
        return NULL;
    }

    sessions->kv = racs_kvstore_create(capacity, racs_sessions_hash, racs_sessions_cmp, racs_sessions_destroy_entry);
    pthread_rwlock_init(&sessions->rwlock, NULL);

    return sessions;
}

racs_uint8 *racs_sessions_get(racs_sessions *sessions, racs_uint64 stream_id) {
    pthread_rwlock_rdlock(&sessions->rwlock);

    racs_uint64 key[2] = {stream_id, 0};
    racs_uint8 *session = racs_kvstore_get(sessions->kv, key);

    pthread_rwlock_unlock(&sessions->rwlock);

    return session;
}

void racs_sessions_put(racs_sessions *sessions, racs_uint64 stream_id, racs_uint8 *session) {
    pthread_rwlock_wrlock(&sessions->rwlock);

    racs_uint64 *key = malloc(2 * sizeof(racs_uint64));
    if (!key) {
        racs_log_error("Failed to allocate key.");
        return;
    }

    key[0] = stream_id;
    key[1] = 0;

    racs_kvstore_put(sessions->kv, key, session);
    pthread_rwlock_unlock(&sessions->rwlock);
}

void racs_sessions_delete(racs_sessions *sessions, racs_uint64 stream_id) {
    pthread_rwlock_wrlock(&sessions->rwlock);

    racs_uint64 key[2] = {stream_id, 0};
    racs_kvstore_delete(sessions->kv, key);

    pthread_rwlock_unlock(&sessions->rwlock);
}

void racs_sessions_destroy(racs_sessions *sessions) {
    pthread_rwlock_wrlock(&sessions->rwlock);
    racs_kvstore_destroy(sessions->kv);
    pthread_rwlock_unlock(&sessions->rwlock);
    pthread_rwlock_destroy(&sessions->rwlock);
}

racs_uint64 racs_sessions_hash(void *key) {
    racs_uint64 hash[2];
    murmur3_x64_128(key, 2 * sizeof(racs_uint64), 0, hash);
    return hash[0];
}

int racs_sessions_cmp(void *a, void *b) {
    racs_uint64 *x = (racs_uint64 *) a;
    racs_uint64 *y = (racs_uint64 *) b;
    return x[0] == y[0];
}

void racs_sessions_destroy_entry(void *key, void *value) {
    free(key);
    free(value);
}

int racs_sessions_id_cmp(const racs_uint8 *src, const racs_uint8 *dest) {
    return uuid_compare(src, dest) == 0;
}

off_t racs_session_read(racs_session *session, racs_uint8 *buf) {
    off_t offset = 0;

    memcpy(session->id, buf, 16);
    offset += 16;

    offset = racs_read_uint16(&session->channels, buf, offset);
    offset = racs_read_uint16(&session->bit_depth, buf, offset);
    offset = racs_read_uint32(&session->sample_rate, buf, offset);
    offset = racs_read_uint64((racs_uint64 *) &session->ref, buf, offset);
    offset = racs_read_uint64(&session->stream_id, buf, offset);

    return offset;
}

off_t racs_session_write(racs_uint8 *buf, racs_session *session) {
    off_t offset = 0;

    memcpy(buf, session->id, 16);
    offset += 16;

    offset = racs_write_uint16(buf, session->channels, offset);
    offset = racs_write_uint16(buf, session->bit_depth, offset);
    offset = racs_write_uint32(buf, session->sample_rate, offset);
    offset = racs_write_uint64(buf, session->ref, offset);
    offset = racs_write_uint64(buf, session->stream_id, offset);

    return offset;
}
