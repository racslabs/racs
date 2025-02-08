#include "memtable.h"

static AUXTS__Memtable* Memtable_construct(int capacity);
static void Memtable_append(AUXTS__Memtable* memtable, uint64_t* key, uint8_t* block, int block_size);
static void Memtable_flush(AUXTS__Memtable* memtable);
static void Memtable_destroy(AUXTS__Memtable* memtable);
static void MemtableEntry_destroy(AUXTS__MemtableEntry* entry);
static AUXTS__SSTable* SSTable_construct(int entry_count);
static void create_time_partitioned_directories(uint64_t milliseconds);
static void SSTable_read_index_entries(AUXTS__SSTable* sstable);
static void SSTable_write(AUXTS__SSTable* sstable, AUXTS__Memtable* memtable);
static off_t SSTable_write_memtable(AUXTS__SSTable* sstable, AUXTS__Memtable* memtable, void* buffer);
static off_t MemtableEntry_write_sstable_index_entry(const AUXTS__MemtableEntry* memtable_entry, AUXTS__SSTableIndexEntry* index_entry, void* buffer, off_t offset);
static off_t SSTable_write_index_entries(AUXTS__SSTable* sstable, void* buffer, off_t offset);
static off_t write_index_entry(void* buffer, AUXTS__SSTableIndexEntry* index_entry, off_t offset);
static off_t buffer_write(void* buffer, void* data, int size, off_t offset);
void SSTable_read_index_entries_in_memory(AUXTS__SSTable* sstable, uint8_t* buffer);


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

AUXTS__SSTable* AUXTS__read_sstable_index_entries_in_memory(uint8_t* buffer, size_t size) {
    uint16_t entry_count;

    memcpy(&entry_count, buffer + (size - AUXTS__TRAILER_SIZE), sizeof(uint16_t));
    entry_count = AUXTS__swap16_if_big_endian(entry_count);

    size_t offset = size - (entry_count * AUXTS__INDEX_ENTRY_SIZE) - AUXTS__TRAILER_SIZE;

    AUXTS__SSTable* sstable = SSTable_construct(entry_count);
    sstable->entry_count = entry_count;
    sstable->fd = -1;

    SSTable_read_index_entries_in_memory(sstable, buffer + offset);

    return sstable;
}

void SSTable_read_index_entries_in_memory(AUXTS__SSTable* sstable, uint8_t* buffer) {
    off_t _offset = 0;

    for (int entry = 0; entry < sstable->entry_count; ++entry) {
        uint64_t block_id;

        memcpy(&block_id, buffer + _offset, sizeof(uint64_t));
        sstable->index_entries[entry].key[0] = AUXTS__swap64_if_big_endian(block_id);
        _offset += sizeof(uint64_t);

        uint64_t timestamp;

        memcpy(&timestamp, buffer + _offset, sizeof(uint64_t));
        sstable->index_entries[entry].key[1] = AUXTS__swap64_if_big_endian(timestamp);
        _offset += sizeof(uint64_t);

        uint64_t offset;

        memcpy(&offset, buffer + _offset, sizeof(uint64_t));
        sstable->index_entries[entry].offset = AUXTS__swap64_if_big_endian(offset);
        _offset += sizeof(uint64_t);
    }

}

AUXTS__SSTable* AUXTS__read_sstable_index_entries(const char* filename) {
    int fd = open(filename, O_RDONLY);
    if (fd == -1) {
        perror("Failed to open AUXTS__SSTable");
        exit(EXIT_FAILURE);
    }

    off_t file_size = lseek(fd, 0, SEEK_END);
    lseek(fd, file_size - AUXTS__TRAILER_SIZE, SEEK_SET);

    uint16_t entry_count;

    read(fd, &entry_count, sizeof(uint16_t));
    entry_count = AUXTS__swap16_if_big_endian(entry_count);

    AUXTS__SSTable* sstable = SSTable_construct(entry_count);
    sstable->entry_count = entry_count;
    sstable->size = file_size;
    sstable->fd = fd;
    sstable->buffer = malloc(file_size + 1);

    lseek(fd, 0, SEEK_SET);
    read(fd, sstable->buffer, file_size);

    lseek(fd, file_size - (entry_count * AUXTS__INDEX_ENTRY_SIZE) - AUXTS__TRAILER_SIZE, SEEK_SET);
    SSTable_read_index_entries(sstable);

    return sstable;
}

void SSTable_read_index_entries(AUXTS__SSTable* sstable) {
    for (int entry = 0; entry < sstable->entry_count; ++entry) {
        uint64_t  block_id;

        read(sstable->fd, &block_id, sizeof(uint64_t));
        sstable->index_entries[entry].key[0] = AUXTS__swap64_if_big_endian(block_id);

        uint64_t timestamp;

        read(sstable->fd, &timestamp, sizeof(uint64_t));
        sstable->index_entries[entry].key[1] = AUXTS__swap64_if_big_endian(timestamp);

        uint64_t offset;

        read(sstable->fd, &offset, sizeof(uint64_t));
        sstable->index_entries[entry].offset = AUXTS__swap64_if_big_endian(offset);
    }
}

void SSTable_write(AUXTS__SSTable* sstable, AUXTS__Memtable* memtable) {
    void* buffer;

    uint16_t entry_count = memtable->size;
    size_t size = AUXTS__HEADER_SIZE + (entry_count * (AUXTS__MAX_BLOCK_SIZE + AUXTS__MEMTABLE_ENTRY_METADATA_SIZE + AUXTS__INDEX_ENTRY_SIZE)) + AUXTS__TRAILER_SIZE;

    if (posix_memalign(&buffer, AUXTS__BLOCK_ALIGN, size) != 0) {
        perror("Buffer allocation failed");
        exit(EXIT_FAILURE);
    }

    char path[64];

    uint64_t milliseconds = memtable->entries[0].key[1];
    create_time_partitioned_directories(milliseconds);
    AUXTS__get_time_partitioned_path(milliseconds, path);

    sstable->fd = open(path, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (sstable->fd == -1) {
        perror("Failed to open AUXTS__SSTable");
        exit(EXIT_FAILURE);
    }

    sstable->entry_count = entry_count;

    off_t offset;
    offset = SSTable_write_memtable(sstable, memtable, buffer);
    offset = SSTable_write_index_entries(sstable, buffer, offset);

    size_t file_size = AUXTS__swap64_if_big_endian(offset);

    buffer_write(buffer, &file_size, sizeof(size_t), 0);
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

void AUXTS__get_time_partitioned_path(uint64_t milliseconds, char* path) {
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
    off_t offset = AUXTS__HEADER_SIZE;

    for (int entry = 0; entry < sstable->entry_count; ++entry) {
        AUXTS__MemtableEntry* memtable_entry = &memtable->entries[entry];
        AUXTS__SSTableIndexEntry* index_entry = &sstable->index_entries[entry];
        offset = MemtableEntry_write_sstable_index_entry(memtable_entry, index_entry, buffer, offset);
    }

    return offset;
}

// value        byte-order      bytes
// -----------------------------------------
// block-size   little-endian   2
// block-id     little-endian   8
// timestamp    little-endian   8
// block        little-endian   <block-size>
off_t MemtableEntry_write_sstable_index_entry(const AUXTS__MemtableEntry* memtable_entry, AUXTS__SSTableIndexEntry* index_entry, void* buffer, off_t offset) {
    uint16_t block_size = AUXTS__swap16_if_big_endian(memtable_entry->block_size);
    index_entry->offset = offset;
    offset = buffer_write(buffer, &block_size, sizeof(uint16_t), offset);

    uint64_t block_id = AUXTS__swap64_if_big_endian(memtable_entry->key[0]);
    index_entry->key[0] = memtable_entry->key[0];
    offset = buffer_write(buffer, &block_id, sizeof(uint64_t), offset);

    uint64_t timestamp = AUXTS__swap64_if_big_endian(memtable_entry->key[1]);
    index_entry->key[1] = memtable_entry->key[1];
    offset = buffer_write(buffer, &timestamp, sizeof(uint64_t), offset);

    return buffer_write(buffer, memtable_entry->block, memtable_entry->block_size, offset);
}

off_t write_index_entry(void* buffer, AUXTS__SSTableIndexEntry* index_entry, off_t offset) {
    uint64_t block_id = AUXTS__swap64_if_big_endian(index_entry->key[0]);
    offset = buffer_write(buffer, &block_id, sizeof(uint64_t), offset);

    uint64_t timestamp = AUXTS__swap64_if_big_endian(index_entry->key[1]);
    offset = buffer_write(buffer, &timestamp, sizeof(uint64_t), offset);

    size_t _offset = AUXTS__swap64_if_big_endian(index_entry->offset);
    return buffer_write(buffer, &_offset, sizeof(size_t), offset);
}

off_t SSTable_write_index_entries(AUXTS__SSTable* sstable, void* buffer, off_t offset) {
    for (int entry = 0; entry < sstable->entry_count; ++entry) {
        offset = write_index_entry(buffer, &sstable->index_entries[entry], offset);
    }

    uint16_t entry_count = AUXTS__swap16_if_big_endian(sstable->entry_count);
    return buffer_write(buffer, &entry_count, sizeof(uint16_t), offset);
}

AUXTS__MemtableEntry* AUXTS__read_memtable_entry(uint8_t* buffer, size_t offset) {
    AUXTS__MemtableEntry* entry = malloc(sizeof(AUXTS__MemtableEntry));
    if (!entry) {
        perror("Failed to allocate AUXTS__MemtableEntry");
        exit(EXIT_FAILURE);
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

off_t buffer_write(void* buffer, void* data, int size, off_t offset) {
    memcpy(((uint8_t*)buffer) + offset, data, size);
    return offset + size;
}

int test_multi_memtable() {
    AUXTS__MultiMemtable* mmt = AUXTS__MultiMemtable_construct(2);

    uint64_t key0[2];
    uint64_t key1[2];

    AUXTS__murmur3_x64_128("stream1", 7, 0, key0);
    key0[1] = AUXTS__get_milliseconds();

    printf("id %llu\n", key0[0]);

    AUXTS__MultiMemtable_append(mmt, key0, "1234", 4);

    AUXTS__murmur3_x64_128("stream1", 7, 0, key1);
    key1[1] = AUXTS__get_milliseconds() + 1;

    AUXTS__MultiMemtable_append(mmt, key1, "5678", 4);

    AUXTS__murmur3_x64_128("stream1", 7, 0, key1);
    key1[1] = AUXTS__get_milliseconds() + 2;

    AUXTS__MultiMemtable_append(mmt, key1, "1111", 4);

    AUXTS__MultiMemtable_destroy(mmt);

    char* path = ".data/2025/02/07/15/32/55/377.df";

    AUXTS__SSTable* sstable = AUXTS__read_sstable_index_entries(path);

    AUXTS__SSTable* sstable2 = AUXTS__read_sstable_index_entries_in_memory(sstable->buffer, sstable->size);

    size_t offset = sstable2->index_entries[1].offset;
    AUXTS__MemtableEntry* entry = AUXTS__read_memtable_entry(sstable->buffer, offset);

    printf("%s\n", entry->block);







    return 0;
}
