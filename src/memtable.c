#include "memtable.h"

rats_multi_memtable *rats_multi_memtable_create(int num_tables, int capacity) {
    rats_multi_memtable *mmt = malloc(sizeof(rats_multi_memtable));
    if (!mmt) {
        perror("Failed to allocate mmt");
        return NULL;
    }

    mmt->index = 0;
    mmt->num_tables = num_tables;
    pthread_mutex_init(&mmt->mutex, NULL);

    mmt->tables = malloc(num_tables * sizeof(rats_memtable));
    if (!mmt->tables) {
        perror("Failed to allocate rats_memtable to mmt");
        free(mmt);
        return NULL;
    }

    for (int i = 0; i < num_tables; ++i) {
        mmt->tables[i] = rats_memtable_create(capacity);
    }

    return mmt;
}

void rats_multi_memtable_append(rats_multi_memtable *mmt, rats_uint64 *key, rats_uint8 *block, rats_uint16 block_size) {
    if (!mmt) return;

    rats_memtable *mt = mmt->tables[mmt->index];
    if (!mt) {
        perror("rats_memtable cannot be null");
        return;
    }

    pthread_mutex_lock(&mmt->mutex);

    rats_memtable_append(mt, key, block, block_size);
    mmt->index = ++mmt->index % mmt->num_tables;

    pthread_mutex_unlock(&mmt->mutex);
}

void rats_multi_memtable_destroy(rats_multi_memtable *mmt) {
    if (!mmt) return;

    pthread_mutex_lock(&mmt->mutex);
    rats_multi_memtable_flush(mmt);

    for (int i = 0; i < mmt->num_tables; ++i) {
        rats_memtable *memtable = mmt->tables[i];
        rats_memtable_destroy(memtable);
    }

    free(mmt->tables);

    pthread_mutex_unlock(&mmt->mutex);
    pthread_mutex_destroy(&mmt->mutex);

    free(mmt);
}

void rats_multi_memtable_flush(rats_multi_memtable *mmt) {
    if (!mmt) return;

    for (int i = 0; i < mmt->num_tables; ++i) {
        rats_memtable *mt = mmt->tables[i];
        rats_memtable_flush(mt);
    }
}

rats_sstable *rats_sstable_read(const char *filename) {
    int fd = open(filename, O_RDONLY);
    if (fd == -1) {
        perror("Failed to open rats_sstable");
        return NULL;
    }

    off_t file_size = lseek(fd, 0, SEEK_END);
    if (file_size == -1) {
        perror("Failed to determine file capacity");
        close(fd);
        return NULL;
    }

    rats_uint16 entry_count;
    if (pread(fd, &entry_count, sizeof(rats_uint16), file_size - RATS_TRAILER_SIZE) != sizeof(rats_uint16)) {
        perror("Failed to read entry num_files");
        close(fd);
        return NULL;
    }

    rats_sstable *sstable = rats_sstable_create(entry_count);
    if (!sstable) {
        close(fd);
        return NULL;
    }

    sstable->num_entries = entry_count;
    sstable->size = file_size;
    sstable->fd = fd;
    sstable->data = malloc(file_size);

    if (!sstable->data) {
        perror("Failed to allocate data");
        free(sstable);
        close(fd);
        return NULL;
    }

    if (pread(fd, sstable->data, file_size, 0) != file_size) {
        perror("Failed to read file contents");
        free(sstable->data);
        free(sstable);
        close(fd);
        return NULL;
    }

    rats_sstable_read_index_entries(sstable);
    return sstable;
}

rats_sstable *rats_sstable_read_in_memory(rats_uint8 *data, size_t size) {
    rats_uint16 num_entries;

    memcpy(&num_entries, data + (size - RATS_TRAILER_SIZE), sizeof(rats_uint16));
    size_t offset = size - (num_entries * RATS_INDEX_ENTRY_SIZE) - RATS_TRAILER_SIZE;

    rats_sstable *sst = rats_sstable_create(num_entries);
    if (!sst) return NULL;

    sst->num_entries = num_entries;
    sst->fd = -1;

    rats_sstable_read_index_entries_in_memory(sst, data + offset);

    return sst;
}

void rats_sstable_destroy_except_data(rats_sstable *sst) {
    free(sst->index_entries);
    free(sst);
}

rats_memtable_entry *rats_memtable_entry_read(rats_uint8 *buf, size_t offset) {
    rats_memtable_entry *entry = malloc(sizeof(rats_memtable_entry));
    if (!entry) {
        perror("Failed to allocate rats_memtable_entry");
        return NULL;
    }

    offset = rats_read_uint16(&entry->block_size, buf, (off_t) offset);
    offset = rats_read_uint64(&entry->key[0], buf, (off_t) offset);
    offset = rats_read_uint64(&entry->key[1], buf, (off_t) offset);

    entry->block = malloc(entry->block_size);
    memcpy(entry->block, buf + offset, entry->block_size);

    return entry;
}

rats_memtable *rats_memtable_create(int capacity) {
    rats_memtable *mt = malloc(sizeof(rats_memtable));
    if (!mt) {
        perror("Failed to allocate mt");
        return NULL;
    }

    mt->num_entries = 0;
    mt->capacity = capacity;

    mt->entries = malloc(sizeof(rats_memtable_entry) * mt->capacity);
    if (!mt->entries) {
        perror("Failed to allocate rats_memtable_entry to mt");
        free(mt);
        return NULL;
    }

    pthread_mutex_init(&mt->mutex, NULL);

    for (int i = 0; i < mt->capacity; ++i) {
        if (posix_memalign((void **) &mt->entries[i].block, RATS__ALIGN, RATS_MAX_BLOCK_SIZE) != 0) {
            perror("Failed to allocate block to mt");
            rats_memtable_destroy(mt);
            return NULL;
        }
    }

    return mt;
}

void rats_memtable_append(rats_memtable *mt, rats_uint64 *key, rats_uint8 *block, rats_uint16 block_size) {
    if (!mt) return;

    pthread_mutex_lock(&mt->mutex);

    if (mt->num_entries >= mt->capacity) {
        rats_memtable_flush(mt);
    }

    memcpy(mt->entries[mt->num_entries].key, key, sizeof(rats_uint64) * 2);
    memcpy(mt->entries[mt->num_entries].block, block, block_size);

    mt->entries[mt->num_entries].block_size = block_size;
    ++mt->num_entries;

    pthread_mutex_unlock(&mt->mutex);
}

void rats_memtable_flush(rats_memtable *mt) {
    if (!mt) return;

    int num_entries = mt->num_entries;
    if (num_entries == 0) {
        return;
    }

    rats_memtable_write(mt);
    mt->num_entries = 0;
}

void rats_memtable_destroy(rats_memtable *mt) {
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

void rats_memtable_write(rats_memtable *mt) {
    rats_sstable *sst = rats_sstable_create(mt->num_entries);
    if (!sst) {
        perror("sst cannot be null");
        return;
    }

    size_t size = RATS_HEADER_SIZE +
                  (mt->num_entries *
                   (RATS_MAX_BLOCK_SIZE + RATS_MEMTABLE_ENTRY_METADATA_SIZE + RATS_INDEX_ENTRY_SIZE)) +
                  RATS_TRAILER_SIZE;

    rats_uint8 *buf = rats_allocate_buffer(size, sst);
    if (!buf) {
        rats_sstable_destroy_except_data(sst);
        return;
    }

    char path[64];
    rats_uint64 timestamp = mt->entries[0].key[1];
    rats_sstable_path((rats_int64) timestamp, path);

    sst->num_entries = mt->num_entries;
    if (rats_sstable_open(path, sst) == -1) {
        rats_sstable_destroy_except_data(sst);
        free(buf);
        return;
    }

    off_t offset;
    offset = rats_memtable_to_sstable(buf, sst, mt);
    offset = rats_sstable_index_entries_write(buf, sst, offset);

    rats_sstable_write(buf, sst, offset);

    free(buf);
    rats_sstable_destroy_except_data(sst);
}

void rats_sstable_path(rats_int64 timestamp, char *path) {
    rats_time_create_dirs(timestamp);
    rats_time_to_path(timestamp, path);
}

rats_uint8 *rats_allocate_buffer(size_t size, rats_sstable *sst) {
    rats_uint8 *buf;
    if (posix_memalign((void *) &buf, RATS_BLOCK_ALIGN, size) != 0) {
        perror("Buffer allocation failed");
        return NULL;
    }
    return buf;
}

int rats_sstable_open(const char *path, rats_sstable *sst) {
    sst->fd = open(path, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (sst->fd == -1) {
        perror("Failed to open sst");
        return -1;
    }
    return 0;
}

void rats_sstable_write(rats_uint8 *buf, rats_sstable *sst, size_t offset) {
    rats_write_uint64(buf, offset, 0);
    write(sst->fd, buf, offset);
}

rats_sstable *rats_sstable_create(int num_entries) {
    rats_sstable *sst = malloc(sizeof(rats_sstable));
    if (!sst) {
        perror("Failed to allocate sst");
        return NULL;
    }

    sst->index_entries = malloc(sizeof(rats_sstable_index_entry) * num_entries);
    if (!sst->index_entries) {
        perror("Failed to allocate rats_sstable_index_entry to sst");
        free(sst);
        return NULL;
    }

    return sst;
}

void rats_sstable_read_index_entries_in_memory(rats_sstable *sst, rats_uint8 *data) {
    off_t offset = 0;

    for (int entry = 0; entry < sst->num_entries; ++entry) {
        rats_sstable_index_entry *index_entry = &sst->index_entries[entry];
        offset = rats_read_uint64(&index_entry->key[0], data, offset);
        offset = rats_read_uint64(&index_entry->key[1], data, offset);
        offset = rats_read_uint64((rats_uint64 *) &index_entry->offset, data, offset);
    }
}

void rats_sstable_read_index_entries(rats_sstable *sst) {
    for (int entry = 0; entry < sst->num_entries; ++entry) {
        rats_sstable_index_entry *index_entry = &sst->index_entries[entry];

        rats_io_read_uint64(&index_entry->key[0], sst->fd);
        rats_io_read_uint64(&index_entry->key[1], sst->fd);
        rats_io_read_uint64((rats_uint64 *) &index_entry->offset, sst->fd);
    }
}

off_t rats_memtable_to_sstable(rats_uint8 *buf, rats_sstable *sst, rats_memtable *mt) {
    off_t offset = RATS_HEADER_SIZE;

    for (int entry = 0; entry < sst->num_entries; ++entry) {
        rats_memtable_entry *mt_entry = &mt->entries[entry];
        rats_sstable_index_entry *index_entry = &sst->index_entries[entry];
        rats_sstable_index_entry_update(index_entry, mt_entry, offset);
        offset = rats_memtable_entry_write(buf, mt_entry, offset);
    }

    return offset;
}

off_t rats_sstable_index_entries_write(rats_uint8 *buf, rats_sstable *sst, off_t offset) {
    for (int entry = 0; entry < sst->num_entries; ++entry) {
        offset = rats_write_index_entry(buf, &sst->index_entries[entry], offset);
    }
    return rats_write_uint16(buf, sst->num_entries, offset);
}

void
rats_sstable_index_entry_update(rats_sstable_index_entry *index_entry, rats_memtable_entry *mt_entry, off_t offset) {
    index_entry->offset = offset;
    index_entry->key[0] = mt_entry->key[0];
    index_entry->key[1] = mt_entry->key[1];
}

off_t rats_memtable_entry_write(rats_uint8 *buf, const rats_memtable_entry *mt_entry, off_t offset) {
    offset = rats_write_uint16(buf, mt_entry->block_size, offset);
    offset = rats_write_uint64(buf, mt_entry->key[0], offset);
    offset = rats_write_uint64(buf, mt_entry->key[1], offset);
    return rats_write_bin(buf, mt_entry->block, mt_entry->block_size, offset);
}

off_t rats_write_index_entry(rats_uint8 *buf, rats_sstable_index_entry *index_entry, off_t offset) {
    offset = rats_write_uint64(buf, index_entry->key[0], offset);
    offset = rats_write_uint64(buf, index_entry->key[1], offset);
    return rats_write_uint64(buf, index_entry->offset, offset);
}
