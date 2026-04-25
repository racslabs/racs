
#include "offsets.h"

racs_uint64 racs_offsets_hash(void *key) {
    racs_uint64 hash[2];
    murmur3_x64_128(key, 2 * sizeof(racs_uint64), 0, hash);
    return hash[0];
}

int racs_offsets_cmp(void *a, void *b) {
    racs_uint64 *x = a;
    racs_uint64 *y = b;
    return x[0] == y[0];
}

void racs_offsets_destroy_entry(void *key, void *value) {
    free(key);
    free(value);
}

racs_offsets *racs_offsets_create() {
    racs_offsets *offsets = malloc(sizeof(racs_offsets));
    if (!offsets) {
        racs_log_error("Failed to allocate racs_offsets");
        return NULL;
    }

    offsets->kv = racs_kvstore_create(RACS_MAX_OFFSETS, racs_offsets_hash, racs_offsets_cmp, racs_offsets_destroy_entry);
    pthread_rwlock_init(&offsets->rwlock, NULL);

    return offsets;
}

racs_uint64 racs_offsets_get(racs_offsets *offsets, racs_uint64 stream_id) {
    pthread_rwlock_rdlock(&offsets->rwlock);

    racs_uint64 key[2] = { stream_id, 0 };
    racs_uint64 *offset = racs_kvstore_get(offsets->kv, key);

    pthread_rwlock_unlock(&offsets->rwlock);

    if (!offset) return 0;
    return *offset;
}

void racs_offsets_put(racs_offsets *offsets, racs_uint64 stream_id, racs_uint64 offset) {
    pthread_rwlock_wrlock(&offsets->rwlock);

    racs_uint64 *key = malloc(2 * sizeof(racs_uint64));
    if (!key) {
        racs_log_error("Failed to allocate key.");
        return;
    }

    key[0] = stream_id;
    key[1] = 0;

    racs_uint64 *_offset = malloc(sizeof(racs_uint64));
    if (!_offset) {
        racs_log_error("Failed to allocate offset");
        return;
    }

    memcpy(_offset, &offset, sizeof(racs_uint64));

    racs_kvstore_put(offsets->kv, key, _offset);
    pthread_rwlock_unlock(&offsets->rwlock);
}

void racs_offsets_init(racs_offsets *offsets) {
    char *dir = NULL;
    char *path1 = NULL;
    char *path2 = NULL;

    asprintf(&dir, "%s/.racs", racs_metadata_dir);
    asprintf(&path1, "%s/.racs/md", racs_metadata_dir);
    asprintf(&path2, "%s/.racs/seg", racs_time_dir);

    mkdir(dir, 0777);
    mkdir(path1, 0777);
    mkdir(path2, 0777);

    racs_filelist *list1 = racs_sorted_filelist(path1);
    racs_filelist *list2 = racs_sorted_filelist(path2);

    free(dir);
    free(path1);
    free(path2);

    for (int i = 0; i < list1->num_files; ++i) {
        racs_uint64 stream_id = racs_path_to_stream_id(list1->files[i]);

        for (int j = 0; j < list2->num_files; ++j) {
            racs_sstable *sst = racs_sstable_read(list2->files[j]);
            if (!sst) continue;

            racs_uint8 *data = sst->data;
            if (!data) {
                racs_sstable_destroy_except_data(sst);
                continue;
            }

            size_t size = sst->size;

            racs_sstable_destroy_except_data(sst);
            sst = racs_sstable_read_in_memory(data, size);
            free(data);

            if (!sst->data) continue;

            for (int k = 0; k < sst->num_entries; ++k) {
                size_t offset = sst->index_entries[k].offset;

                racs_memtable_entry *entry = racs_memtable_entry_read(sst->data, offset);
                if (!entry) continue;

                if (entry->key[0] == stream_id) {
                    racs_uint64 _offset = racs_offsets_get(offsets, stream_id);
                    _offset += entry->block_size;
                    racs_offsets_put(offsets, stream_id, _offset);
                }

                free(entry->block);
                free(entry);
            }

            free(sst->data);
            racs_sstable_destroy_except_data(sst);
        }
    }

    racs_filelist_destroy(list1);
    racs_filelist_destroy(list2);
}

void racs_offsets_destroy(racs_offsets *offsets) {
    pthread_rwlock_wrlock(&offsets->rwlock);
    racs_kvstore_destroy(offsets->kv);
    pthread_rwlock_unlock(&offsets->rwlock);

    pthread_rwlock_destroy(&offsets->rwlock);
}

racs_time racs_offsets_timestamp(racs_uint64 offset, racs_time ref, racs_uint16 channels, racs_uint16 bit_depth, racs_uint32 sample_rate) {
    double seconds = offset / (double) (channels * sample_rate * (bit_depth / 8));
    return (racs_time) (seconds * 1000) + ref;
}
