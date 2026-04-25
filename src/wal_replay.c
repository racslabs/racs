
#include "wal_replay.h"


void racs_wal_replay(racs_multi_memtable *mmt, racs_offsets *offsets, racs_versions *versions, racs_sessions *sessions) {
    char *dir = NULL;

    racs_uint64 checkpoint_lsn = racs_wal_checkpoint_lsn();
    asprintf(&dir, "%s/.racs/wal", racs_wal_dir);
    racs_filelist *list = racs_sorted_filelist(dir);

    free(dir);

    for (int i = 0; i < list->num_files; ++i) {
        if (strcmp(basename(list->files[i]), "manifest") == 0) continue;

        racs_wal_segment *segment = racs_wal_segment_read(list->files[i]);
        if (!segment) continue;

        off_t offset = 0;
        while (offset < segment->size) {
            racs_wal_entry *entry = racs_wal_entry_read(segment->data, &offset);
            if (!entry) {
                racs_log_error("Corrupted racs_wal_entry");
                continue;
            }

            racs_uint32 checksum = crc32c(0, entry->op, entry->size);
            if (checksum != entry->checksum || entry->lsn <= checkpoint_lsn) {
                racs_wal_entry_destroy(entry);
                continue;
            }

            racs_log_info("REPLAY OP: %d LSN: %llu", entry->op_code, entry->lsn);

            if (entry->op_code == RACS_OP_CODE_APPEND)
                racs_wal_replay_append(entry, mmt, offsets, versions, sessions);
            else if (entry->op_code == RACS_OP_CODE_EXPIRE)
                racs_wal_replay_expire(entry, offsets, versions);
            else if (entry->op_code == RACS_OP_CODE_CREATE)
                racs_wal_replay_create(entry);
            else if (entry->op_code == RACS_OP_CODE_OPEN)
                racs_wal_replay_open(entry, sessions);
            else if (entry->op_code == RACS_OP_CODE_CLOSE)
                racs_wal_replay_close(entry, sessions);

            racs_wal_entry_destroy(entry);
        }

        racs_wal_segment_destroy(segment);
    }

    racs_filelist_destroy(list);
}

void racs_wal_replay_append(racs_wal_entry *entry, racs_multi_memtable *mmt, racs_offsets *offsets, racs_versions *versions, racs_sessions *sessions) {
    racs_frame frame;
    racs_frame_parse(entry->op, &frame);

    racs_uint8 *_session = racs_sessions_get(sessions, frame.header.stream_id);
    if (!_session) return;

    racs_session session;
    racs_session_read(&session, _session);

    racs_uint64 offset = racs_offsets_get(offsets, frame.header.stream_id);
    racs_time timestamp = racs_offsets_timestamp(offset, session.ref, session.channels, session.bit_depth, session.sample_rate);
    racs_uint64 version = racs_versions_get(versions, frame.header.stream_id);

    racs_uint64 key[3] = { frame.header.stream_id, timestamp, version };
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
}

void racs_wal_replay_expire(racs_wal_entry *entry, racs_offsets *offsets, racs_versions *versions) {
    racs_uint64 stream_id;
    memcpy(&stream_id, entry->op, sizeof(racs_uint64));

    racs_ttl_delete_stream(offsets, versions, stream_id);
}

void racs_wal_replay_create(racs_wal_entry *entry) {
    racs_metadata metadata;
    racs_metadata_read(&metadata, entry->op);

    racs_uint64 hash = racs_hash(metadata.id);
    racs_metadata_put(&metadata, hash);
    racs_metadata_destroy(&metadata);
}

void racs_wal_replay_open(racs_wal_entry *entry, racs_sessions *sessions) {
    racs_uint8 *_session = malloc(sizeof(racs_session));
    memcpy(_session, entry->op, sizeof(racs_session));

    racs_session session;
    racs_session_read(&session, _session);
    racs_sessions_put(sessions, session.stream_id, _session);
}

void racs_wal_replay_close(racs_wal_entry *entry, racs_sessions *sessions) {
    racs_uint64 stream_id;
    memcpy(&stream_id, entry->op, sizeof(racs_uint64));

    racs_sessions_delete(sessions, stream_id);
}

racs_wal_segment *racs_wal_segment_read(const char *path) {
    racs_wal_segment *segment = malloc(sizeof(racs_wal_segment));
    if (!segment) {
        racs_log_error("Failed to allocate racs_wal_segment");
        return NULL;
    }

    segment->fd = open(path, O_RDONLY);
    if (segment->fd == -1) {
        racs_log_error("Failed to open racs_wal_segment");
        free(segment);
        return NULL;
    }

    segment->size = lseek(segment->fd, 0, SEEK_END);
    if (segment->size <= 0) {
        close(segment->fd);
        free(segment);
        return NULL;
    }

    segment->data = malloc(segment->size);
    if (!segment->data) {
        racs_log_error("Failed to allocate racs_wal_segment data");
        close(segment->fd);
        free(segment);
        return NULL;
    }

    ssize_t rc = pread(segment->fd, segment->data, segment->size, 0);
    if (rc != segment->size) {
        racs_log_error("Failed to read racs_wal segment data");
        close(segment->fd);
        free(segment->data);
        free(segment);
        return NULL;
    }

    return segment;
}

void racs_wal_entry_destroy(racs_wal_entry *entry) {
    free(entry->op);
    free(entry);
}

void racs_wal_segment_destroy(racs_wal_segment *segment) {
    close(segment->fd);
    free(segment->data);
    free(segment);
}

racs_wal_entry *racs_wal_entry_read(racs_uint8 *buf, off_t *offset) {
    racs_wal_entry *entry = malloc(sizeof(racs_wal_entry));
    if (!entry) {
        racs_log_error("Failed to allocate racs_wal_entry");
        return NULL;
    }

    *offset = racs_read_uint32(&entry->op_code, buf, *offset);
    *offset = racs_read_uint32(&entry->checksum, buf, *offset);
    *offset = racs_read_uint64(&entry->size, buf, *offset);

    entry->op = malloc(entry->size);
    if (!entry->op) {
        free(entry);
        return NULL;
    }

    memcpy(entry->op, buf + *offset, entry->size);

    *offset += entry->size;
    *offset = racs_read_uint64(&entry->lsn, buf, *offset);

    return entry;
}