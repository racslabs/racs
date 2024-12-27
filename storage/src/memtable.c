#include "memtable.h"

MultiMemtable* create_multi_memtable(int memtable_capacity) {
    MultiMemtable* multi_memtable = malloc(sizeof(MultiMemtable));

    if (!multi_memtable) {
        perror("Failed to allocate MultiMemtable");
        exit(-1);
    }

    multi_memtable->index = 0;
    pthread_mutex_init(&multi_memtable->mutex, NULL);

    for (int i = 0; i < AUXTS_MAX_NUM_MEMTABLES; ++i) {
        multi_memtable->tables[i] = create_memtable(memtable_capacity);
    }

    return multi_memtable;
}

void append_to_multi_memtable(MultiMemtable* multi_memtable, uint64_t* key, uint8_t* block, int block_size) {
    pthread_mutex_lock(&multi_memtable->mutex);

    Memtable* active_memtable = multi_memtable->tables[multi_memtable->index];
    append_to_memtable(active_memtable, key, block, block_size);

    multi_memtable->index = ++multi_memtable->index % AUXTS_MAX_NUM_MEMTABLES;

    pthread_mutex_unlock(&multi_memtable->mutex);
}

void destroy_multi_memtable(MultiMemtable* multi_memtable) {
    pthread_mutex_lock(&multi_memtable->mutex);

    flush_multi_memtable(multi_memtable);

    for (int i = 0; i < AUXTS_MAX_NUM_MEMTABLES; ++i) {
        destroy_memtable(multi_memtable->tables[i]);
    }

    pthread_mutex_unlock(&multi_memtable->mutex);
    pthread_mutex_destroy(&multi_memtable->mutex);

    free(multi_memtable);
}

void flush_multi_memtable(MultiMemtable* multi_memtable) {
    for (int i = 0; i < AUXTS_MAX_NUM_MEMTABLES; ++i) {
        Memtable* memtable = multi_memtable->tables[i];
        flush_memtable(memtable);
    }
}

Memtable* create_memtable(int capacity) {
    Memtable* memtable = malloc(sizeof(Memtable));

    if (!memtable) {
        perror("Failed to allocate Memtable");
        exit(-1);
    }

    memtable->size = 0;
    memtable->capacity = capacity;
    memtable->entries = malloc(sizeof(MemtableEntry) * memtable->capacity);

    if (!memtable->entries) {
        perror("Failed to allocate MemtableEntry to Memtable");
        exit(-1);
    }

    pthread_mutex_init(&memtable->mutex, NULL);

    for (int i = 0; i < memtable->capacity; ++i) {
        if (posix_memalign((void**)&memtable->entries[i].block, AUXTS_ALIGN, AUXTS_MAX_BLOCK_SIZE) != 0) {
            perror("Failed to allocate block to Memtable");
            exit(-1);
        }
    }

    return memtable;
}

void append_to_memtable(Memtable* memtable, uint64_t* key, uint8_t* block, int block_size) {
    pthread_mutex_lock(&memtable->mutex);

    if (memtable->size >= memtable->capacity) {
        flush_memtable(memtable);
    }

    memcpy(memtable->entries[memtable->size].key, key, sizeof(uint64_t) * 2);
    memcpy(memtable->entries[memtable->size].block, block, block_size);

    memtable->entries[memtable->size].block_size = block_size;
    ++memtable->size;

    pthread_mutex_unlock(&memtable->mutex);
}

void flush_memtable(Memtable* memtable) {
    int entry_count = memtable->size;

    if (entry_count > 0) {
        SSTable *sstable = create_sstable(entry_count);
        write_to_sstable(sstable, memtable);
        destroy_sstable(sstable);

        memtable->size = 0;
    }
}

void destroy_memtable(Memtable* memtable) {
    pthread_mutex_lock(&memtable->mutex);

    for (int i = 0; i < memtable->size; ++i) {
        free(memtable->entries[i].block);
    }

    free(memtable->entries);

    pthread_mutex_unlock(&memtable->mutex);
    pthread_mutex_destroy(&memtable->mutex);

    free(memtable);
}

SSTable* read_sstable(const char* filename) {
    int fd = open(filename, O_RDONLY);

    if (fd == -1) {
        perror("Failed to open SSTable");
        exit(-1);
    }

    off_t file_size = lseek(fd, 0, SEEK_END);
    lseek(fd, file_size - 2, SEEK_SET);

    uint16_t entry_count;

    read(fd, &entry_count, sizeof(uint16_t));
    entry_count = swap16_if_big_endian(entry_count);

    lseek(fd, file_size - (entry_count * 24) - 2, SEEK_SET);

    SSTable* sstable = create_sstable(entry_count);
    sstable->entry_count = entry_count;
    sstable->fd = fd;

    read_index_entries(sstable);

    return sstable;
}

void read_index_entries(SSTable* sstable) {
    for (int entry = 0; entry < sstable->entry_count; ++entry) {
        uint64_t  block_id;

        read(sstable->fd, &block_id, sizeof(uint64_t));
        block_id = swap64_if_big_endian(block_id);
        sstable->index_entries[entry].key[0] = block_id;

        uint64_t timestamp;

        read(sstable->fd, &timestamp, sizeof(uint64_t));
        timestamp = swap64_if_big_endian(timestamp);
        sstable->index_entries[entry].key[1] = timestamp;

        uint64_t offset;

        read(sstable->fd, &offset, sizeof(uint64_t));
        offset = swap64_if_big_endian(offset);
        sstable->index_entries[entry].offset = offset;
    }
}

void write_to_sstable(SSTable* sstable, Memtable* memtable) {
    void* buffer;

    uint16_t entry_count = memtable->size;
    size_t size = entry_count * 65570 + 2;

    if (posix_memalign(&buffer, AUXTS_BLOCK_ALIGN, size) != 0) {
        perror("Buffer allocation failed");
        exit(-1);
    }

    char path[64];

    uint64_t milliseconds = memtable->entries[0].key[1];
    create_time_partitioned_directories(milliseconds);
    get_time_partitioned_path(milliseconds, path);

    sstable->fd = open(path, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (sstable->fd == -1) {
        perror("Failed to open SSTable");
        exit(-1);
    }

    sstable->entry_count = entry_count;

    off_t offset;
    offset = write_memtable_entries_to_sstable(buffer, sstable, memtable);
    offset = write_index_entries_to_sstable(buffer, sstable, offset);

    write(sstable->fd, buffer, offset);

    free(buffer);
}

void create_time_partitioned_directories(uint64_t milliseconds) {
    char dir[32];
    struct tm info;

    milliseconds_to_tm(milliseconds, &info);

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

void get_time_partitioned_path(uint64_t milliseconds, char* path) {
    struct tm info;
    milliseconds_to_tm(milliseconds, &info);

    long remainder = (long)(milliseconds % AUXTS_MILLISECONDS_PER_SECOND);

    sprintf(path, ".data/%d/%02d/%02d/%02d/%02d/%02d/%03ld.df",
            info.tm_year + 1900, info.tm_mon + 1,
            info.tm_mday, info.tm_hour,
            info.tm_min, info.tm_sec,
            remainder);
}

SSTable* create_sstable(int entry_count) {
    SSTable* sstable = malloc(sizeof(SSTable));

    if (!sstable) {
        perror("Failed to allocate SSTable");
        exit(-1);
    }

    sstable->index_entries = malloc(sizeof(SSTableIndexEntry) * entry_count);

    if (!sstable->index_entries) {
        perror("Failed to allocate SSTableIndexEntry to SSTable");
        exit(-1);
    }

    return sstable;
}

void destroy_sstable(SSTable* sstable) {
    free(sstable->index_entries);
    free(sstable);
}

off_t write_memtable_entries_to_sstable(void* buffer, SSTable* sstable, Memtable* memtable) {
    off_t offset = 0;

    for (int entry = 0; entry < sstable->entry_count; ++entry) {
        MemtableEntry* memtable_entry = &memtable->entries[entry];
        SSTableIndexEntry* index_entry = &sstable->index_entries[entry];
        offset = write_memtable_entry(buffer, offset, memtable_entry, index_entry);
    }

    return offset;
}

off_t write_memtable_entry(void* buffer, off_t offset, const MemtableEntry* memtable_entry, SSTableIndexEntry* index_entry) {
    uint16_t block_size = swap16_if_big_endian(memtable_entry->block_size);
    index_entry->offset = offset;
    offset = write_to_buffer(buffer, &block_size, sizeof(uint16_t), offset);

    uint64_t block_id = swap64_if_big_endian(memtable_entry->key[0]);
    index_entry->key[0] = memtable_entry->key[0];
    offset = write_to_buffer(buffer, &block_id, sizeof(uint64_t), offset);

    uint64_t timestamp = swap64_if_big_endian(memtable_entry->key[1]);
    index_entry->key[1] = memtable_entry->key[1];
    offset = write_to_buffer(buffer, &timestamp, sizeof(uint64_t), offset);

    return write_to_buffer(buffer, memtable_entry->block, memtable_entry->block_size, offset);
}

off_t write_index_entry(void* buffer, SSTableIndexEntry* index_entry, off_t offset) {
    uint64_t block_id = index_entry->key[0];
    block_id = swap64_if_big_endian(block_id);

    offset = write_to_buffer(buffer, &block_id, sizeof(uint64_t), offset);

    uint64_t timestamp = index_entry->key[1];
    timestamp = swap64_if_big_endian(timestamp);

    offset = write_to_buffer(buffer, &timestamp, sizeof(uint64_t), offset);

    size_t _offset = index_entry->offset;
    _offset = swap64_if_big_endian(_offset);

    return write_to_buffer(buffer, &_offset, sizeof(size_t), offset);
}

off_t write_index_entries_to_sstable(void* buffer, SSTable* sstable, off_t offset) {
    for (int entry = 0; entry < sstable->entry_count; ++entry) {
        offset = write_index_entry(buffer, &sstable->index_entries[entry], offset);
    }

    uint16_t entry_count = sstable->entry_count;
    entry_count = swap16_if_big_endian(entry_count);

    return write_to_buffer(buffer, &entry_count, sizeof(uint16_t), offset);
}

off_t write_to_buffer(void* buffer, void* data, int size, off_t offset) {
    memcpy((uint8_t*)buffer + offset, data, size);
    return offset + size;
}

int test_multi_memtable() {
    MultiMemtable* mmt = create_multi_memtable(2);

    uint64_t key0[2];
    uint64_t key1[2];

    murmur3_x64_128("stream0", 7, 0, key0);
    key0[1] = get_milliseconds();

    append_to_multi_memtable(mmt, key0, "1234", 4);

    murmur3_x64_128("stream1", 7, 0, key1);
    key1[1] = get_milliseconds() + 1;

    append_to_multi_memtable(mmt, key1, "5678", 4);

    destroy_multi_memtable(mmt);

    return 0;
}
