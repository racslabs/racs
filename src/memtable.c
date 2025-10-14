// RACS - Remote Audio Caching Server
// Copyright (c) 2025 RACS Labs, LLC. All rights reserved.
//
// Licensed under the RACS Source Available License (RACS-SAL-1.0).
// Non-commercial use only. Commercial use requires a paid license.
// Contact: sales@racslabs.com
//
// SPDX-License-Identifier: RACS-SAL-1.0

#include "memtable.h"

racs_multi_memtable *racs_multi_memtable_create(int num_tables, int capacity) {
    racs_multi_memtable *mmt = malloc(sizeof(racs_multi_memtable));
    if (!mmt) {
        racs_log_fatal("Failed to allocate racs_multi_memtable");
        return NULL;
    }

    mmt->index = 0;
    mmt->num_tables = num_tables;
    mmt->head = NULL;
    mmt->tail = NULL;
    pthread_mutex_init(&mmt->mutex, NULL);

    for (int i = 0; i < num_tables; ++i) {
        racs_memtable *mt = racs_memtable_create(capacity);
        racs_multi_memtable_move_to_head(mmt, mt);
    }

    return mmt;
}

void racs_multi_memtable_move_to_head(racs_multi_memtable *mmt, racs_memtable *mt) {
    if (!mt) return;

    mt->prev = NULL;
    mt->next = (struct racs_memtable *) mmt->head;

    if (mmt->head) {
        mmt->head->prev = (struct racs_memtable *) mt;
        mmt->tail = (racs_memtable *) mmt->tail->prev;
        mmt->tail->next = NULL;
    } else {
        mmt->tail = mt;
    }

    mmt->head = mt;
}

void racs_multi_memtable_append(racs_multi_memtable *mmt, racs_uint64 *key, racs_uint8 *block, racs_uint16 block_size, racs_uint32 checksum) {
    if (!mmt) return;

    pthread_mutex_lock(&mmt->mutex);

    racs_memtable *mt = mmt->head;
    int capacity = mt->capacity;

    if (mt->num_entries >= capacity) {
        racs_memtable_flush_async(mmt->tail);

        mt = racs_memtable_create(capacity);
        racs_multi_memtable_move_to_head(mmt, mt);
    }

    racs_memtable_append(mmt->head, key, block, block_size, checksum);
    pthread_mutex_unlock(&mmt->mutex);
}

void racs_multi_memtable_destroy(racs_multi_memtable *mmt) {
    if (!mmt) return;

    pthread_mutex_lock(&mmt->mutex);
    racs_multi_memtable_flush(mmt);

    racs_memtable *mt = mmt->head;
    while (mt) {
        racs_memtable *next = (racs_memtable *) mt->next;
        racs_memtable_destroy(mt);
        mt = next;
    }

    pthread_mutex_unlock(&mmt->mutex);
    pthread_mutex_destroy(&mmt->mutex);

    free(mmt);
}

void racs_multi_memtable_flush(racs_multi_memtable *mmt) {
    if (!mmt) return;

    pthread_mutex_lock(&mmt->mutex);

    racs_memtable *mt = mmt->head;
    while (mt) {
        racs_memtable *next = (racs_memtable *) mt->next;
        racs_memtable_flush(next);
        mt = next;
    }

    pthread_mutex_unlock(&mmt->mutex);
}

racs_sstable *racs_sstable_read(const char *filename) {
    int fd = open(filename, O_RDONLY);
    if (fd == -1) {
        racs_log_error("Failed to open racs_sstable");
        return NULL;
    }

    off_t file_size = lseek(fd, 0, SEEK_END);
    if (file_size == -1) {
        racs_log_error("Failed to read file entries");
        close(fd);
        return NULL;
    }

    racs_uint16 entry_count;
    if (pread(fd, &entry_count, sizeof(racs_uint16), file_size - RACS_TRAILER_SIZE) != sizeof(racs_uint16)) {
        racs_log_error("Failed to read entry_count");
        close(fd);
        return NULL;
    }

    racs_sstable *sstable = racs_sstable_create(entry_count);
    if (!sstable) {
        close(fd);
        return NULL;
    }

    sstable->num_entries = entry_count;
    sstable->size = file_size;
    sstable->fd = fd;
    sstable->data = malloc(file_size);

    if (!sstable->data) {
        racs_log_error("Failed to allocate racs_sstable data");
        free(sstable);
        close(fd);
        return NULL;
    }

    if (pread(fd, sstable->data, file_size, 0) != file_size) {
        racs_log_error("Failed to read racs_sstable data");
        free(sstable->data);
        free(sstable);
        close(fd);
        return NULL;
    }

    racs_sstable_read_index_entries(sstable);
    return sstable;
}

racs_sstable *racs_sstable_read_in_memory(racs_uint8 *data, size_t size) {
    racs_uint16 num_entries;

    memcpy(&num_entries, data + (size - RACS_TRAILER_SIZE), sizeof(racs_uint16));
    size_t offset = size - (num_entries * RACS_INDEX_ENTRY_SIZE) - RACS_TRAILER_SIZE;

    racs_sstable *sst = racs_sstable_create(num_entries);
    if (!sst) return NULL;

    sst->num_entries = num_entries;
    sst->fd = -1;

    racs_sstable_read_index_entries_in_memory(sst, data + offset);

    return sst;
}

void racs_sstable_destroy_except_data(racs_sstable *sst) {
    free(sst->index_entries);
    free(sst);
}

racs_memtable_entry *racs_memtable_entry_read(racs_uint8 *buf, size_t offset) {
    racs_memtable_entry *entry = malloc(sizeof(racs_memtable_entry));
    if (!entry) {
        racs_log_error("Failed to allocate racs_memtable_entry");
        return NULL;
    }

    offset = racs_read_uint64(&entry->key[0], buf, (off_t) offset);
    offset = racs_read_uint64(&entry->key[1], buf, (off_t) offset);
    offset = racs_read_uint32(&entry->checksum, buf, (off_t) offset);
    offset = racs_read_uint16(&entry->block_size, buf, (off_t) offset);

    entry->block = malloc(entry->block_size);
    memcpy(entry->block, buf + offset, entry->block_size);

    if (crc32c(0, entry->block, entry->block_size) != entry->checksum) {
        free(entry->block);
        free(entry);

        return NULL;
    }

    return entry;
}

racs_memtable *racs_memtable_create(int capacity) {
    racs_memtable *mt = malloc(sizeof(racs_memtable));
    if (!mt) {
        racs_log_error("Failed to allocate racs_memtable");
        return NULL;
    }

    mt->num_entries = 0;
    mt->capacity = capacity;

    mt->entries = malloc(sizeof(racs_memtable_entry) * mt->capacity);
    if (!mt->entries) {
        racs_log_error("Failed to allocate racs_memtable_entry to racs_memtable");
        free(mt);
        return NULL;
    }

    pthread_mutex_init(&mt->mutex, NULL);

    for (int i = 0; i < mt->capacity; ++i) {
        if (posix_memalign((void **) &mt->entries[i].block, RACS_ALIGN, RACS_MAX_BLOCK_SIZE) != 0) {
            racs_log_error("Failed to allocate block to racs_memtable");
            racs_memtable_destroy(mt);
            return NULL;
        }
    }

    return mt;
}

void racs_memtable_append(racs_memtable *mt, racs_uint64 *key, racs_uint8 *block, racs_uint16 block_size, racs_uint32 checksum) {
    if (!mt) return;

    pthread_mutex_lock(&mt->mutex);

    memcpy(mt->entries[mt->num_entries].key, key, sizeof(racs_uint64) * 2);
    memcpy(mt->entries[mt->num_entries].block, block, block_size);

    mt->entries[mt->num_entries].block_size = block_size;
    mt->entries[mt->num_entries].checksum = checksum;
    ++mt->num_entries;

    pthread_mutex_unlock(&mt->mutex);
}

void racs_memtable_flush(racs_memtable *mt) {
    if (!mt) return;

    int num_entries = mt->num_entries;
    if (num_entries == 0) return;

    racs_memtable_write(mt);
    mt->num_entries = 0;

    racs_memtable_destroy(mt);
}

void racs_memtable_flush_async(racs_memtable *mt) {
    pthread_t thread;

    if (pthread_create(&thread, NULL, (void *(*)(void *))racs_memtable_flush, mt) != 0) {
        racs_log_error("pthread_create failed");
        return;
    }

    pthread_detach(thread);
}

void racs_memtable_destroy(racs_memtable *mt) {
    if (!mt) return;

    pthread_mutex_lock(&mt->mutex);

    for (int i = 0; i < mt->num_entries; ++i) {
        free(mt->entries[i].block);
    }

    free(mt->entries);

    pthread_mutex_unlock(&mt->mutex);
    pthread_mutex_destroy(&mt->mutex);

    free(mt);
}

void racs_memtable_write(racs_memtable *mt) {
    racs_sstable *sst = racs_sstable_create(mt->num_entries);
    if (!sst) {
        racs_log_error("racs_sstable cannot be null");
        return;
    }

    size_t size = RACS_HEADER_SIZE +
                  (mt->num_entries *
                   (RACS_MAX_BLOCK_SIZE + RACS_MEMTABLE_ENTRY_METADATA_SIZE + RACS_INDEX_ENTRY_SIZE)) +
                  RACS_TRAILER_SIZE;

    racs_uint8 *buf = racs_allocate_buffer(size, sst);
    if (!buf) {
        racs_sstable_destroy_except_data(sst);
        return;
    }

    char *path = NULL;
    racs_uint64 timestamp = mt->entries[0].key[1];
    racs_sstable_path((racs_int64) timestamp, &path);

    sst->num_entries = mt->num_entries;
    if (racs_sstable_open(path, sst) == -1) {
        racs_sstable_destroy_except_data(sst);
        free(buf);
        free(path);
        return;
    }

    off_t offset;
    offset = racs_memtable_to_sstable(buf, sst, mt);
    offset = racs_sstable_index_entries_write(buf, sst, offset);

    racs_sstable_write(buf, sst, offset);

    free(buf);
    free(path);
    racs_sstable_destroy_except_data(sst);
}

void racs_sstable_path(racs_int64 timestamp, char **path) {
    racs_time_create_dirs(timestamp);
    racs_time_to_path(timestamp, path);
}

racs_uint8 *racs_allocate_buffer(size_t size, racs_sstable *sst) {
    racs_uint8 *buf;
    if (posix_memalign((void *) &buf, RACS_BLOCK_ALIGN, size) != 0) {
        racs_log_error("Buffer allocation failed");
        return NULL;
    }
    return buf;
}

int racs_sstable_open(const char *path, racs_sstable *sst) {
    sst->fd = open(path, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (sst->fd == -1) {
        racs_log_error("Failed to open racs_sstable");
        return -1;
    }
    return 0;
}

void racs_sstable_write(racs_uint8 *buf, racs_sstable *sst, size_t offset) {
    racs_write_uint64(buf, offset, 0);
    racs_write_uint32(buf, RACS_VERSION, 8);
    racs_write_uint32(buf, 0, 12);

    write(sst->fd, buf, offset);
}

racs_sstable *racs_sstable_create(int num_entries) {
    racs_sstable *sst = malloc(sizeof(racs_sstable));
    if (!sst) {
        racs_log_error("Failed to allocate racs_sstable");
        return NULL;
    }

    sst->index_entries = malloc(sizeof(racs_sstable_index_entry) * num_entries);
    if (!sst->index_entries) {
        racs_log_error("Failed to allocate racs_sstable_index_entry to racs_sstable");
        free(sst);
        return NULL;
    }

    return sst;
}

void racs_sstable_read_index_entries_in_memory(racs_sstable *sst, racs_uint8 *data) {
    off_t offset = 0;

    for (int entry = 0; entry < sst->num_entries; ++entry) {
        racs_sstable_index_entry *index_entry = &sst->index_entries[entry];
        offset = racs_read_uint64(&index_entry->key[0], data, offset);
        offset = racs_read_uint64(&index_entry->key[1], data, offset);
        offset = racs_read_uint64((racs_uint64 *) &index_entry->offset, data, offset);
    }
}

void racs_sstable_read_index_entries(racs_sstable *sst) {
    for (int entry = 0; entry < sst->num_entries; ++entry) {
        racs_sstable_index_entry *index_entry = &sst->index_entries[entry];

        racs_io_read_uint64(&index_entry->key[0], sst->fd);
        racs_io_read_uint64(&index_entry->key[1], sst->fd);
        racs_io_read_uint64((racs_uint64 *) &index_entry->offset, sst->fd);
    }
}

off_t racs_memtable_to_sstable(racs_uint8 *buf, racs_sstable *sst, racs_memtable *mt) {
    off_t offset = RACS_HEADER_SIZE;

    for (int entry = 0; entry < sst->num_entries; ++entry) {
        racs_memtable_entry *mt_entry = &mt->entries[entry];
        racs_sstable_index_entry *index_entry = &sst->index_entries[entry];
        racs_sstable_index_entry_update(index_entry, mt_entry, offset);
        offset = racs_memtable_entry_write(buf, mt_entry, offset);
    }

    return offset;
}

off_t racs_sstable_index_entries_write(racs_uint8 *buf, racs_sstable *sst, off_t offset) {
    for (int entry = 0; entry < sst->num_entries; ++entry) {
        offset = racs_write_index_entry(buf, &sst->index_entries[entry], offset);
    }
    return racs_write_uint16(buf, sst->num_entries, offset);
}

void
racs_sstable_index_entry_update(racs_sstable_index_entry *index_entry, racs_memtable_entry *mt_entry, off_t offset) {
    index_entry->offset = offset;
    index_entry->key[0] = mt_entry->key[0];
    index_entry->key[1] = mt_entry->key[1];
}

off_t racs_memtable_entry_write(racs_uint8 *buf, const racs_memtable_entry *mt_entry, off_t offset) {
    offset = racs_write_uint64(buf, mt_entry->key[0], offset);
    offset = racs_write_uint64(buf, mt_entry->key[1], offset);
    offset = racs_write_uint32(buf, mt_entry->checksum, offset);
    offset = racs_write_uint16(buf, mt_entry->block_size, offset);
    return racs_write_bin(buf, mt_entry->block, mt_entry->block_size, offset);
}

off_t racs_write_index_entry(racs_uint8 *buf, racs_sstable_index_entry *index_entry, off_t offset) {
    offset = racs_write_uint64(buf, index_entry->key[0], offset);
    offset = racs_write_uint64(buf, index_entry->key[1], offset);
    return racs_write_uint64(buf, index_entry->offset, offset);
}
