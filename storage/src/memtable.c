#include "memtable.h"

static AUXTS__Memtable* Memtable_construct(int capacity);
static void Memtable_append(AUXTS__Memtable* memtable, uint64_t* key, uint8_t* block, int block_size);
static void Memtable_flush(AUXTS__Memtable* memtable);
static void Memtable_destroy(AUXTS__Memtable* memtable);
static AUXTS__SSTable* SSTable_construct(int entry_count);
static void create_time_partitioned_directories(uint64_t milliseconds);
static void get_time_partitioned_path(uint64_t milliseconds, char* path);
static void SSTable_read_index_entries(AUXTS__SSTable* sstable);
static void SSTable_write(AUXTS__SSTable* sstable, AUXTS__Memtable* memtable);
static off_t SSTable_write_memtable(AUXTS__SSTable* sstable, AUXTS__Memtable* memtable, void* buffer);
static off_t MemtableEntry_write_sstable_index_entry(const AUXTS__MemtableEntry* memtable_entry, AUXTS__SSTableIndexEntry* index_entry, void* buffer, off_t offset);
static off_t SSTable_write_index_entries(AUXTS__SSTable* sstable, void* buffer, off_t offset);
static off_t write_index_entry(void* buffer, AUXTS__SSTableIndexEntry* index_entry, off_t offset);
static off_t write_to_buffer(void* buffer, void* data, int size, off_t offset);


AUXTS__MultiMemtable* AUXTS__MultiMemtable_construct(int capacity) {
    AUXTS__MultiMemtable* multi_memtable = malloc(sizeof(AUXTS__MultiMemtable));

    if (!multi_memtable) {
        perror("Failed to allocate AUXTS__MultiMemtable");
        exit(EXIT_FAILURE);
    }

    multi_memtable->index = 0;
    pthread_mutex_init(&multi_memtable->mutex, NULL);

    for (int i = 0; i < AUXTS__MAX_NUM_MEMTABLES; ++i) {
        multi_memtable->tables[i] = Memtable_construct(capacity);
    }

    return multi_memtable;
}

void AUXTS__MultiMemtable_append(AUXTS__MultiMemtable* multi_memtable, uint64_t* key, uint8_t* block, int block_size) {
    pthread_mutex_lock(&multi_memtable->mutex);

    AUXTS__Memtable* active_memtable = multi_memtable->tables[multi_memtable->index];
    Memtable_append(active_memtable, key, block, block_size);

    multi_memtable->index = ++multi_memtable->index % AUXTS__MAX_NUM_MEMTABLES;

    pthread_mutex_unlock(&multi_memtable->mutex);
}

void AUXTS__MultiMemtable_destroy(AUXTS__MultiMemtable* multi_memtable) {
    pthread_mutex_lock(&multi_memtable->mutex);

    AUXTS__MultiMemtable_flush(multi_memtable);

    for (int i = 0; i < AUXTS__MAX_NUM_MEMTABLES; ++i) {
        Memtable_destroy(multi_memtable->tables[i]);
    }

    pthread_mutex_unlock(&multi_memtable->mutex);
    pthread_mutex_destroy(&multi_memtable->mutex);

    free(multi_memtable);
}

void AUXTS__MultiMemtable_flush(AUXTS__MultiMemtable* multi_memtable) {
    for (int i = 0; i < AUXTS__MAX_NUM_MEMTABLES; ++i) {
        AUXTS__Memtable* memtable = multi_memtable->tables[i];
        Memtable_flush(memtable);
    }
}

AUXTS__Memtable* Memtable_construct(int capacity) {
    AUXTS__Memtable* memtable = malloc(sizeof(AUXTS__Memtable));

    if (!memtable) {
        perror("Failed to allocate AUXTS__Memtable");
        exit(EXIT_FAILURE);
    }

    memtable->size = 0;
    memtable->capacity = capacity;
    memtable->entries = malloc(sizeof(AUXTS__MemtableEntry) * memtable->capacity);

    if (!memtable->entries) {
        perror("Failed to allocate AUXTS__MemtableEntry to AUXTS__Memtable");
        exit(EXIT_FAILURE);
    }

    pthread_mutex_init(&memtable->mutex, NULL);

    for (int i = 0; i < memtable->capacity; ++i) {
        if (posix_memalign((void**)&memtable->entries[i].block, AUXTS_ALIGN, AUXTS__MAX_BLOCK_SIZE) != 0) {
            perror("Failed to allocate block to AUXTS__Memtable");
            exit(EXIT_FAILURE);
        }
    }

    return memtable;
}

void Memtable_append(AUXTS__Memtable* memtable, uint64_t* key, uint8_t* block, int block_size) {
    pthread_mutex_lock(&memtable->mutex);

    if (memtable->size >= memtable->capacity) {
        Memtable_flush(memtable);
    }

    memcpy(memtable->entries[memtable->size].key, key, sizeof(uint64_t) * 2);
    memcpy(memtable->entries[memtable->size].block, block, block_size);

    memtable->entries[memtable->size].block_size = block_size;
    ++memtable->size;

    pthread_mutex_unlock(&memtable->mutex);
}

void Memtable_flush(AUXTS__Memtable* memtable) {
    int entry_count = memtable->size;

    if (entry_count > 0) {
        AUXTS__SSTable *sstable = SSTable_construct(entry_count);
        SSTable_write(sstable, memtable);
        AUXTS__SSTable_destroy(sstable);

        memtable->size = 0;
    }
}

void Memtable_destroy(AUXTS__Memtable* memtable) {
    pthread_mutex_lock(&memtable->mutex);

    for (int i = 0; i < memtable->size; ++i) {
        free(memtable->entries[i].block);
    }

    free(memtable->entries);

    pthread_mutex_unlock(&memtable->mutex);
    pthread_mutex_destroy(&memtable->mutex);

    free(memtable);
}

AUXTS__SSTable* AUXTS__read_sstable(const char* filename) {
    int fd = open(filename, O_RDONLY);

    if (fd == -1) {
        perror("Failed to open AUXTS__SSTable");
        exit(EXIT_FAILURE);
    }

    off_t file_size = lseek(fd, 0, SEEK_END);
    lseek(fd, file_size - 2, SEEK_SET);

    uint16_t entry_count;

    read(fd, &entry_count, sizeof(uint16_t));
    entry_count = AUXTS__swap16_if_big_endian(entry_count);

    lseek(fd, file_size - (entry_count * 24) - 2, SEEK_SET);

    AUXTS__SSTable* sstable = SSTable_construct(entry_count);
    sstable->entry_count = entry_count;
    sstable->fd = fd;

    SSTable_read_index_entries(sstable);

    return sstable;
}

void SSTable_read_index_entries(AUXTS__SSTable* sstable) {
    for (int entry = 0; entry < sstable->entry_count; ++entry) {
        uint64_t  block_id;

        read(sstable->fd, &block_id, sizeof(uint64_t));
        block_id = AUXTS__swap64_if_big_endian(block_id);
        sstable->index_entries[entry].key[0] = block_id;

        uint64_t timestamp;

        read(sstable->fd, &timestamp, sizeof(uint64_t));
        timestamp = AUXTS__swap64_if_big_endian(timestamp);
        sstable->index_entries[entry].key[1] = timestamp;

        uint64_t offset;

        read(sstable->fd, &offset, sizeof(uint64_t));
        offset = AUXTS__swap64_if_big_endian(offset);
        sstable->index_entries[entry].offset = offset;
    }
}

void SSTable_write(AUXTS__SSTable* sstable, AUXTS__Memtable* memtable) {
    void* buffer;

    uint16_t entry_count = memtable->size;
    size_t size = entry_count * 65570 + 2;

    if (posix_memalign(&buffer, AUXTS__BLOCK_ALIGN, size) != 0) {
        perror("Buffer allocation failed");
        exit(EXIT_FAILURE);
    }

    char path[64];

    uint64_t milliseconds = memtable->entries[0].key[1];
    create_time_partitioned_directories(milliseconds);
    get_time_partitioned_path(milliseconds, path);

    sstable->fd = open(path, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (sstable->fd == -1) {
        perror("Failed to open AUXTS__SSTable");
        exit(EXIT_FAILURE);
    }

    sstable->entry_count = entry_count;

    off_t offset;
    offset = SSTable_write_memtable(sstable, memtable, buffer);
    offset = SSTable_write_index_entries(sstable, buffer, offset);

    write(sstable->fd, buffer, offset);

    free(buffer);
}

void create_time_partitioned_directories(uint64_t milliseconds) {
    char dir[32];
    struct tm info;

    AUXTS__milliseconds_to_tm(milliseconds, &info);

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
    AUXTS__milliseconds_to_tm(milliseconds, &info);

    long remainder = (long)(milliseconds % AUXTS__MILLISECONDS_PER_SECOND);

    sprintf(path, ".data/%d/%02d/%02d/%02d/%02d/%02d/%03ld.df",
            info.tm_year + 1900, info.tm_mon + 1,
            info.tm_mday, info.tm_hour,
            info.tm_min, info.tm_sec,
            remainder);
}

AUXTS__SSTable* SSTable_construct(int entry_count) {
    AUXTS__SSTable* sstable = malloc(sizeof(AUXTS__SSTable));

    if (!sstable) {
        perror("Failed to allocate AUXTS__SSTable");
        exit(EXIT_FAILURE);
    }

    sstable->index_entries = malloc(sizeof(AUXTS__SSTableIndexEntry) * entry_count);

    if (!sstable->index_entries) {
        perror("Failed to allocate AUXTS__SSTableIndexEntry to AUXTS__SSTable");
        exit(EXIT_FAILURE);
    }

    return sstable;
}

void AUXTS__SSTable_destroy(AUXTS__SSTable* sstable) {
    free(sstable->index_entries);
    free(sstable);
}

off_t SSTable_write_memtable(AUXTS__SSTable* sstable, AUXTS__Memtable* memtable, void* buffer) {
    off_t offset = 0;

    for (int entry = 0; entry < sstable->entry_count; ++entry) {
        AUXTS__MemtableEntry* memtable_entry = &memtable->entries[entry];
        AUXTS__SSTableIndexEntry* index_entry = &sstable->index_entries[entry];
        offset = MemtableEntry_write_sstable_index_entry(memtable_entry, index_entry, buffer, offset);
    }

    return offset;
}

off_t MemtableEntry_write_sstable_index_entry(const AUXTS__MemtableEntry* memtable_entry, AUXTS__SSTableIndexEntry* index_entry, void* buffer, off_t offset) {
    uint16_t block_size = AUXTS__swap16_if_big_endian(memtable_entry->block_size);
    index_entry->offset = offset;
    offset = write_to_buffer(buffer, &block_size, sizeof(uint16_t), offset);

    uint64_t block_id = AUXTS__swap64_if_big_endian(memtable_entry->key[0]);
    index_entry->key[0] = memtable_entry->key[0];
    offset = write_to_buffer(buffer, &block_id, sizeof(uint64_t), offset);

    uint64_t timestamp = AUXTS__swap64_if_big_endian(memtable_entry->key[1]);
    index_entry->key[1] = memtable_entry->key[1];
    offset = write_to_buffer(buffer, &timestamp, sizeof(uint64_t), offset);

    return write_to_buffer(buffer, memtable_entry->block, memtable_entry->block_size, offset);
}

off_t write_index_entry(void* buffer, AUXTS__SSTableIndexEntry* index_entry, off_t offset) {
    uint64_t block_id = index_entry->key[0];
    block_id = AUXTS__swap64_if_big_endian(block_id);

    offset = write_to_buffer(buffer, &block_id, sizeof(uint64_t), offset);

    uint64_t timestamp = index_entry->key[1];
    timestamp = AUXTS__swap64_if_big_endian(timestamp);

    offset = write_to_buffer(buffer, &timestamp, sizeof(uint64_t), offset);

    size_t _offset = index_entry->offset;
    _offset = AUXTS__swap64_if_big_endian(_offset);

    return write_to_buffer(buffer, &_offset, sizeof(size_t), offset);
}

off_t SSTable_write_index_entries(AUXTS__SSTable* sstable, void* buffer, off_t offset) {
    for (int entry = 0; entry < sstable->entry_count; ++entry) {
        offset = write_index_entry(buffer, &sstable->index_entries[entry], offset);
    }

    uint16_t entry_count = sstable->entry_count;
    entry_count = AUXTS__swap16_if_big_endian(entry_count);

    return write_to_buffer(buffer, &entry_count, sizeof(uint16_t), offset);
}

off_t write_to_buffer(void* buffer, void* data, int size, off_t offset) {
    memcpy((uint8_t*)buffer + offset, data, size);
    return offset + size;
}

int test_multi_memtable() {
    AUXTS__MultiMemtable* mmt = AUXTS__MultiMemtable_construct(2);

    uint64_t key0[2];
    uint64_t key1[2];

    AUXTS__murmur3_x64_128("stream0", 7, 0, key0);
    key0[1] = AUXTS__get_milliseconds();

    AUXTS__MultiMemtable_append(mmt, key0, "1234", 4);

    AUXTS__murmur3_x64_128("stream1", 7, 0, key1);
    key1[1] = AUXTS__get_milliseconds() + 1;

    AUXTS__MultiMemtable_append(mmt, key1, "5678", 4);

    AUXTS__MultiMemtable_destroy(mmt);

    return 0;
}
