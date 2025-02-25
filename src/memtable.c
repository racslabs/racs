#include "memtable.h"

static void memtable_append(auxts_memtable* mt, uint64_t* key, uint8_t* block, int block_size);
static void memtable_flush(auxts_memtable* mt);
static void memtable_destroy(auxts_memtable* mt);
static void create_time_partitioned_directories(uint64_t milliseconds);
static void sstable_read_index_entries(auxts_sstable* sst);
static void sstable_write(auxts_sstable* sst, auxts_memtable* mt);
static void sstable_read_index_entries_in_memory(auxts_sstable* sst, uint8_t* data);
static auxts_sstable* sstable_create(int num_entries);
static auxts_memtable* memtable_create(int capacity);
static off_t sstable_write_memtable(auxts_sstable* sst, auxts_memtable* mt, void* buffer);
static off_t memtable_entry_write_sstable_index_entry(const auxts_memtable_entry* mt_entry, auxts_sstable_index_entry* index_entry, void* buffer, off_t offset);
static off_t sstable_write_index_entries(auxts_sstable* sst, void* buffer, off_t offset);
static off_t write_index_entry(void* buffer, auxts_sstable_index_entry* index_entry, off_t offset);
static off_t buffer_write(void* buffer, void* data, int size, off_t offset);

auxts_multi_memtable* auxts_multi_memtable_create(int num_tables, int capacity) {
    auxts_multi_memtable* mmt = malloc(sizeof(auxts_multi_memtable));
    if (!mmt) {
        perror("Failed to allocate mmt");
        return NULL;
    }

    mmt->index = 0;
    mmt->num_tables = num_tables;
    pthread_mutex_init(&mmt->mutex, NULL);

    mmt->tables = malloc(num_tables * sizeof(auxts_memtable));
    if (!mmt->tables) {
        perror("Failed to allocate auxts_memtable to mmt");
        free(mmt);
        return NULL;
    }

    for (int i = 0; i < num_tables; ++i) {
        mmt->tables[i] = memtable_create(capacity);
    }

    return mmt;
}

void auxts_multi_memtable_append(auxts_multi_memtable* mmt, uint64_t* key, uint8_t* block, int block_size) {
    if (!mmt) {
        return;
    }

    pthread_mutex_lock(&mmt->mutex);

    auxts_memtable* active_mt = mmt->tables[mmt->index];
    if (!active_mt) {
        perror("auxts_memtable cannot be null");
        return;
    }

    memtable_append(active_mt, key, block, block_size);
    mmt->index = ++mmt->index % mmt->num_tables;

    pthread_mutex_unlock(&mmt->mutex);
}

void auxts_multi_memtable_destroy(auxts_multi_memtable* mmt) {
    if (!mmt) {
        return;
    }

    pthread_mutex_lock(&mmt->mutex);

    auxts_multi_memtable_flush(mmt);

    for (int i = 0; i < mmt->num_tables; ++i) {
        auxts_memtable* memtable = mmt->tables[i];
        memtable_destroy(memtable);
    }

    free(mmt->tables);

    pthread_mutex_unlock(&mmt->mutex);
    pthread_mutex_destroy(&mmt->mutex);

    free(mmt);
}

void auxts_multi_memtable_flush(auxts_multi_memtable* mmt) {
    if (!mmt) {
        return;
    }

    for (int i = 0; i < mmt->num_tables; ++i) {
        auxts_memtable* mt = mmt->tables[i];
        memtable_flush(mt);
    }
}

auxts_sstable* auxts_sstable_read_index_entries(const char* filename) {
    int fd = open(filename, O_RDONLY);
    if (fd == -1) {
        perror("Failed to open auxts_sstable");
        return NULL;
    }

    off_t file_size = lseek(fd, 0, SEEK_END);
    if (file_size == -1) {
        perror("Failed to determine file num_buckets");
        close(fd);
        return NULL;
    }

    uint16_t entry_count;
    if (pread(fd, &entry_count, sizeof(uint16_t), file_size - AUXTS_TRAILER_SIZE) != sizeof(uint16_t)) {
        perror("Failed to read entry num_files");
        close(fd);
        return NULL;
    }

    entry_count = auxts_swap16_if_big_endian(entry_count);
    auxts_sstable* sstable = sstable_create(entry_count);
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

    sstable_read_index_entries(sstable);
    return sstable;
}

auxts_sstable* auxts_sstable_read_index_entries_in_memory(uint8_t* data, size_t size) {
    uint16_t num_entries;

    memcpy(&num_entries, data + (size - AUXTS_TRAILER_SIZE), sizeof(uint16_t));
    num_entries = auxts_swap16_if_big_endian(num_entries);

    size_t offset = size - (num_entries * AUXTS_INDEX_ENTRY_SIZE) - AUXTS_TRAILER_SIZE;

    auxts_sstable* sst = sstable_create(num_entries);
    if (!sst) {
        return NULL;
    }

    sst->num_entries = num_entries;
    sst->fd = -1;

    sstable_read_index_entries_in_memory(sst, data + offset);

    return sst;
}

void auxts_sstable_destroy_except_data(auxts_sstable* sst) {
    free(sst->index_entries);
    free(sst);
}

auxts_memtable_entry* auxts_memtable_read_entry(uint8_t* buffer, size_t offset) {
    auxts_memtable_entry* entry = malloc(sizeof(auxts_memtable_entry));
    if (!entry) {
        perror("Failed to allocate auxts_memtable_entry");
        return NULL;
    }

    memcpy(&entry->block_size, buffer + offset, sizeof(uint16_t));
    offset += sizeof(uint16_t);

    memcpy(&entry->key[0], buffer + offset, sizeof(uint64_t));
    offset += sizeof(uint64_t);

    memcpy(&entry->key[1], buffer + offset, sizeof(uint64_t));
    offset += sizeof(uint64_t);

    entry->block = malloc(entry->block_size);
    memcpy(entry->block, buffer + offset , entry->block_size);

    return entry;
}

auxts_memtable* memtable_create(int capacity) {
    auxts_memtable* mt = malloc(sizeof(auxts_memtable));
    if (!mt) {
        perror("Failed to allocate mt");
        return NULL;
    }

    mt->num_entries = 0;
    mt->capacity = capacity;

    mt->entries = malloc(sizeof(auxts_memtable_entry) * mt->capacity);
    if (!mt->entries) {
        perror("Failed to allocate auxts_memtable_entry to mt");
        free(mt);
        return NULL;
    }

    pthread_mutex_init(&mt->mutex, NULL);

    for (int i = 0; i < mt->capacity; ++i) {
        if (posix_memalign((void**)&mt->entries[i].block, AUXTS__ALIGN, AUXTS_MAX_BLOCK_SIZE) != 0) {
            perror("Failed to allocate block to mt");
            memtable_destroy(mt);
            return NULL;
        }
    }

    return mt;
}

void memtable_append(auxts_memtable* mt, uint64_t* key, uint8_t* block, int block_size) {
    if (!mt) {
        return;
    }

    pthread_mutex_lock(&mt->mutex);

    if (mt->num_entries >= mt->capacity) {
        memtable_flush(mt);
    }

    memcpy(mt->entries[mt->num_entries].key, key, sizeof(uint64_t) * 2);
    memcpy(mt->entries[mt->num_entries].block, block, block_size);

    mt->entries[mt->num_entries].block_size = block_size;
    ++mt->num_entries;

    pthread_mutex_unlock(&mt->mutex);
}

void memtable_flush(auxts_memtable* mt) {
    if (!mt) {
        return;
    }

    int num_entries = mt->num_entries;
    if (num_entries == 0) {
        return;
    }

    auxts_sstable* sst = sstable_create(num_entries);
    if (!sst) {
        perror("sst cannot be null");
        return;
    }

    sstable_write(sst, mt);
    auxts_sstable_destroy_except_data(sst);

    mt->num_entries = 0;
}

void memtable_destroy(auxts_memtable* mt) {
    if (!mt) {
        return;
    }

    pthread_mutex_lock(&mt->mutex);

    for (int i = 0; i < mt->num_entries; ++i) {
        free(mt->entries[i].block);
    }

    free(mt->entries);

    pthread_mutex_unlock(&mt->mutex);
    pthread_mutex_destroy(&mt->mutex);

    free(mt);
}

void sstable_read_index_entries_in_memory(auxts_sstable* sst, uint8_t* data) {
    off_t _offset = 0;

    for (int entry = 0; entry < sst->num_entries; ++entry) {
        uint64_t block_id;

        memcpy(&block_id, data + _offset, sizeof(uint64_t));
        sst->index_entries[entry].key[0] = auxts_swap64_if_big_endian(block_id);
        _offset += sizeof(uint64_t);

        uint64_t timestamp;

        memcpy(&timestamp, data + _offset, sizeof(uint64_t));
        sst->index_entries[entry].key[1] = auxts_swap64_if_big_endian(timestamp);
        _offset += sizeof(uint64_t);

        uint64_t offset;

        memcpy(&offset, data + _offset, sizeof(uint64_t));
        sst->index_entries[entry].offset = auxts_swap64_if_big_endian(offset);
        _offset += sizeof(uint64_t);
    }

}

void sstable_read_index_entries(auxts_sstable* sst) {
    for (int entry = 0; entry < sst->num_entries; ++entry) {
        uint64_t  block_id;

        read(sst->fd, &block_id, sizeof(uint64_t));
        sst->index_entries[entry].key[0] = auxts_swap64_if_big_endian(block_id);

        uint64_t timestamp;

        read(sst->fd, &timestamp, sizeof(uint64_t));
        sst->index_entries[entry].key[1] = auxts_swap64_if_big_endian(timestamp);

        uint64_t offset;

        read(sst->fd, &offset, sizeof(uint64_t));
        sst->index_entries[entry].offset = auxts_swap64_if_big_endian(offset);
    }
}

void sstable_write(auxts_sstable* sst, auxts_memtable* mt) {
    if (!sst) {
        return;
    }

    void* buffer;

    uint16_t num_entries = mt->num_entries;
    size_t size = AUXTS_HEADER_SIZE + (num_entries * (AUXTS_MAX_BLOCK_SIZE + AUXTS_MEMTABLE_ENTRY_METADATA_SIZE + AUXTS_INDEX_ENTRY_SIZE)) + AUXTS_TRAILER_SIZE;

    if (posix_memalign(&buffer, AUXTS_BLOCK_ALIGN, size) != 0) {
        perror("Buffer allocation failed");
        return;
    }

    char path[64];

    uint64_t milliseconds = mt->entries[0].key[1];
    create_time_partitioned_directories(milliseconds);
    auxts_get_time_partitioned_path(milliseconds, path);

    sst->fd = open(path, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (sst->fd == -1) {
        perror("Failed to open sst");
        return;
    }

    sst->num_entries = num_entries;

    off_t offset;
    offset = sstable_write_memtable(sst, mt, buffer);
    offset = sstable_write_index_entries(sst, buffer, offset);

    size_t file_size = auxts_swap64_if_big_endian(offset);

    buffer_write(buffer, &file_size, sizeof(size_t), 0);
    write(sst->fd, buffer, offset);

    free(buffer);
}

void create_time_partitioned_directories(uint64_t milliseconds) {
    char dir[32];
    struct tm info;

    auxts_milliseconds_to_tm(milliseconds, &info);

    sprintf(dir, ".data");
    mkdir(dir, 0777);

    sprintf(dir, "%s/%d", dir, info.tm_year + 1900);
    mkdir(dir, 0777);

    sprintf(dir, "%s/%02d", dir, info.tm_mon + 1);
    mkdir(dir, 0777);

    sprintf(dir, "%s/%02d", dir, info.tm_mday);
    mkdir(dir, 0777);

    sprintf(dir, "%s/%02d", dir, info.tm_hour);
    mkdir(dir, 0777);

    sprintf(dir, "%s/%02d", dir, info.tm_min);
    mkdir(dir, 0777);

    sprintf(dir, "%s/%02d", dir, info.tm_sec);
    mkdir(dir, 0777);
}

auxts_sstable* sstable_create(int num_entries) {
    auxts_sstable* sst = malloc(sizeof(auxts_sstable));
    if (!sst) {
        perror("Failed to allocate sst");
        return NULL;
    }

    sst->index_entries = malloc(sizeof(auxts_sstable_index_entry) * num_entries);
    if (!sst->index_entries) {
        perror("Failed to allocate auxts_sstable_index_entry to sst");
        free(sst);
        return NULL;
    }

    return sst;
}

off_t sstable_write_memtable(auxts_sstable* sst, auxts_memtable* mt, void* buffer) {
    off_t offset = AUXTS_HEADER_SIZE;

    for (int entry = 0; entry < sst->num_entries; ++entry) {
        auxts_memtable_entry* memtable_entry = &mt->entries[entry];
        auxts_sstable_index_entry* index_entry = &sst->index_entries[entry];
        offset = memtable_entry_write_sstable_index_entry(memtable_entry, index_entry, buffer, offset);
    }

    return offset;
}

// value        byte-order      bytes
// -----------------------------------------
// block-size   little-endian   2
// block-id     little-endian   8
// timestamp    little-endian   8
// block        little-endian   <block-size>
off_t memtable_entry_write_sstable_index_entry(const auxts_memtable_entry* mt_entry, auxts_sstable_index_entry* index_entry, void* buffer, off_t offset) {
    uint16_t block_size = auxts_swap16_if_big_endian(mt_entry->block_size);
    index_entry->offset = offset;
    offset = buffer_write(buffer, &block_size, sizeof(uint16_t), offset);

    uint64_t block_id = auxts_swap64_if_big_endian(mt_entry->key[0]);
    index_entry->key[0] = mt_entry->key[0];
    offset = buffer_write(buffer, &block_id, sizeof(uint64_t), offset);

    uint64_t timestamp = auxts_swap64_if_big_endian(mt_entry->key[1]);
    index_entry->key[1] = mt_entry->key[1];
    offset = buffer_write(buffer, &timestamp, sizeof(uint64_t), offset);

    return buffer_write(buffer, mt_entry->block, mt_entry->block_size, offset);
}

off_t write_index_entry(void* buffer, auxts_sstable_index_entry* index_entry, off_t offset) {
    uint64_t block_id = auxts_swap64_if_big_endian(index_entry->key[0]);
    offset = buffer_write(buffer, &block_id, sizeof(uint64_t), offset);

    uint64_t timestamp = auxts_swap64_if_big_endian(index_entry->key[1]);
    offset = buffer_write(buffer, &timestamp, sizeof(uint64_t), offset);

    size_t _offset = auxts_swap64_if_big_endian(index_entry->offset);
    return buffer_write(buffer, &_offset, sizeof(size_t), offset);
}

off_t sstable_write_index_entries(auxts_sstable* sst, void* buffer, off_t offset) {
    for (int entry = 0; entry < sst->num_entries; ++entry) {
        offset = write_index_entry(buffer, &sst->index_entries[entry], offset);
    }

    uint16_t entry_count = auxts_swap16_if_big_endian(sst->num_entries);
    return buffer_write(buffer, &entry_count, sizeof(uint16_t), offset);
}

off_t buffer_write(void* buffer, void* data, int size, off_t offset) {
    memcpy(((uint8_t*)buffer) + offset, data, size);
    return offset + size;
}
