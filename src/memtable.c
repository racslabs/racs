// RACS - Remote Audio Caching Server
// Copyright (c) 2025 RACS Labs, LLC. All rights reserved.
//
// Licensed under the RACS Source Available License (RACS-SAL-1.0).
// Non-commercial use only. Commercial use requires a paid license.
// Contact: sales@racslabs.com
//

#include "memtable.h"
#include "murmurhash3.h"


typedef struct {
    int capacity;
    racs_dict *dict;
} racs_memtable_partitions;


static racs_uint8 *racs_memtable_to_sstable(racs_memtable *mt, size_t *sst_size);

static racs_uint64 racs_memtable_partitions_hash_callback(const void *key);

static int racs_memtable_partitions_eq_callback(const void *a, const void *b);

static void racs_memtable_partitions_destroy_callback(void *key, void *value);

static racs_memtable_partitions *racs_memtable_partitions_create(int capacity);


racs_memtable *racs_memtable_create(int capacity) {
    racs_memtable *mt = malloc(sizeof(racs_memtable));
    if (!mt) {
        return NULL;
    }

    mt->num_entries = 0;
    mt->capacity = capacity;
    mt->next = NULL;
    mt->prev = NULL;

    mt->entries = calloc(mt->capacity, sizeof(racs_memtable_entry));
    if (!mt->entries) {
        free(mt);
        return NULL;
    }

    pthread_mutex_init(&mt->mutex, NULL);
    return mt;
}

void racs_memtable_append(racs_memtable *mt,
                          const racs_uint64 *key,
                          const racs_uint8 *block,
                          racs_uint16 block_size,
                          racs_uint32 checksum,
                          racs_uint64 lsn) {
    if (!mt) {
        return;
    }

    pthread_mutex_lock(&mt->mutex);

    if (mt->num_entries == mt->capacity) {
        pthread_mutex_unlock(&mt->mutex);
        return;
    }

    mt->entries[mt->num_entries].block = malloc(block_size);
    if (!mt->entries[mt->num_entries].block) {
        pthread_mutex_unlock(&mt->mutex);
        return;
    }

    memcpy(mt->entries[mt->num_entries].key, key, sizeof(racs_uint64) * 3);
    mt->entries[mt->num_entries].lsn = lsn;
    mt->entries[mt->num_entries].block_size = block_size;
    mt->entries[mt->num_entries].checksum = checksum;

    memcpy(mt->entries[mt->num_entries].block, block, block_size);
    ++mt->num_entries;

    pthread_mutex_unlock(&mt->mutex);
}

void racs_memtable_flush(racs_memtable *mt, const char *path) {
    if (!mt || mt->num_entries == 0) {
        return;
    }

    size_t sst_size = 0;
    racs_uint8 *sst = racs_memtable_to_sstable(mt, &sst_size);
    if (!sst) {
        return;
    }

    char tmp_path[PATH_MAX];
    snprintf(tmp_path, sizeof(tmp_path), "%s.tmp", path);

    int fd = open(tmp_path, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd != -1) {
        if (write(fd, sst, sst_size) == (ssize_t)sst_size) {
            fsync(fd);
            close(fd);

            if (rename(tmp_path, path) != 0) {
                unlink(tmp_path);
            }
        } else {
            close(fd);
            unlink(tmp_path);
        }
    }

    free(sst);
}

void racs_memtable_destroy(racs_memtable *mt) {
    if (!mt) {
        return;
    }

    pthread_mutex_lock(&mt->mutex);

    if (mt->entries) {
        for (int i = 0; i < mt->num_entries; ++i) {
            if (mt->entries[i].block) {
                free(mt->entries[i].block);
            }
        }

        free(mt->entries);
    }

    pthread_mutex_unlock(&mt->mutex);
    pthread_mutex_destroy(&mt->mutex);

    free(mt);
}

racs_uint8 *racs_memtable_to_sstable(racs_memtable *mt, size_t *sst_size) {
    size_t data_size = 0;
    for (int i = 0; i < mt->num_entries; i++) {
        data_size += mt->entries[i].block_size;
    }

    size_t index_size = mt->num_entries * sizeof(racs_sstable_index_entry);
    size_t trailer_size = sizeof(racs_uint16);
    size_t total_size = data_size + index_size + trailer_size;

    racs_uint8 *sst = malloc(total_size);
    if (!sst) {
        return NULL;
    }

    racs_uint8 *data_ptr = sst;
    racs_sstable_index_entry *index_ptr = (racs_sstable_index_entry *)(sst + data_size);

    for (int i = 0; i < mt->num_entries; i++) {
        racs_memtable_entry *entry = &mt->entries[i];

        racs_uint32 offset = (racs_uint32)(data_ptr - sst);
        memcpy(data_ptr, entry->block, entry->block_size);
        data_ptr += entry->block_size;

        memcpy(index_ptr[i].key, entry->key, sizeof(racs_uint64) * 3);
        index_ptr[i].offset = offset;
        index_ptr[i].block_size = entry->block_size;
        index_ptr[i].checksum = entry->checksum;
    }

    memcpy(sst + (total_size - trailer_size), &mt->num_entries, trailer_size);
    *sst_size = total_size;

    return sst;
}

racs_memtable_partitions *racs_memtable_partitions_create(int capacity) {
    racs_memtable_partitions *partitions = malloc(sizeof(racs_memtable_partitions));
    if (!partitions) {
        return NULL;
    }

    racs_dict_callbacks callbacks = {
        .hash = racs_memtable_partitions_hash_callback,
        .eq = racs_memtable_partitions_eq_callback,
        .destroy = racs_memtable_partitions_destroy_callback
    };

    partitions->capacity = capacity;
    partitions->dict = racs_dict_create(capacity, callbacks);
    if (!partitions->dict) {
        free(partitions);
        return NULL;
    }

    return partitions;
}

void racs_memtable_partitions_append(racs_memtable_partitions *partitions,
                                     const racs_uint64 *key,
                                     const racs_uint8 *block,
                                     racs_uint16 block_size,
                                     racs_uint32 checksum,
                                     racs_uint64 lsn) {
    if (!partitions || !partitions->dict) {
        return;
    }

    racs_uint64 *partition_key = malloc(2 * sizeof(racs_uint64));
    if (!partition_key) {
        return;
    }

    partition_key[0] = key[0]; // stream-id
    partition_key[1] = key[2]; // version

    racs_memtable *mt = racs_dict_get(partitions->dict, partition_key);
    if (!mt) {
        mt = racs_memtable_create(partitions->capacity);
        racs_dict_put(partitions->dict, partition_key, mt);
    } else {
        free(partition_key);
    }

    racs_memtable_append(mt, key, block, block_size, checksum, lsn);
}

racs_uint64 racs_memtable_partitions_hash_callback(const void *key) {
    racs_uint64 hash[2];
    racs_murmurhash3_x64_128(key, 2 * sizeof(racs_uint64), 0, hash);
    return hash[0];
}

int racs_memtable_partitions_eq_callback(const void *a, const void *b) {
    racs_uint64 *x = (racs_uint64 *) a;
    racs_uint64 *y = (racs_uint64 *) b;
    return x[0] == y[0] && x[1] == y[1];
}

void racs_memtable_partitions_destroy_callback(void *key, void *value) {
    free(key);
    racs_memtable_destroy(value);
}
