
#include "wal_replay.h"

void racs_wal_replay(racs_multi_memtable *mmt, racs_offsets *offsets) {
    char *dir = NULL;

    racs_uint64 checkpoint_lsn = racs_wal_checkpoint_lsn();
    asprintf(&dir, "%s/.racs/wal", racs_wal_dir);
    racs_filelist *list = get_sorted_filelist(dir);

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

            racs_frame frame;
            racs_frame_parse(entry->op, &frame);

            racs_streaminfo streaminfo;
            int rc = racs_streaminfo_get(&streaminfo, frame.header.stream_id);
            if (rc == 0) {
                racs_wal_entry_destroy(entry);
                continue;
            }

            racs_uint64 _offset = racs_offsets_get(offsets, frame.header.stream_id);
            racs_time timestamp = racs_streaminfo_timestamp(&streaminfo, offset);

            racs_uint64 key[2] = { frame.header.stream_id, timestamp };
            racs_multi_memtable_append(mmt, key, frame.pcm_block, frame.header.block_size, frame.header.checksum);

            _offset += frame.header.block_size;
            racs_offsets_put(offsets, frame.header.stream_id, _offset);

            racs_streaminfo_destroy(&streaminfo);
            racs_wal_entry_destroy(entry);
        }

        racs_wal_segment_destroy(segment);
    }

    racs_filelist_destroy(list);
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