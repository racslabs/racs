#include "memtable.h"

static Memtable* memtable_create(int capacity);
static void memtable_append(Memtable* memtable, uint64_t* key, uint8_t* block, int block_size);
static void memtable_flush(Memtable* memtable);
static void memtable_destroy(Memtable* memtable);
static SSTable* sstable_construct(int num_entries);
static void create_time_partitioned_directories(uint64_t milliseconds);
static void sstable_read_index_entries(SSTable* sstable);
static void sstable_write(SSTable* sstable, Memtable* memtable);
static off_t sstable_write_memtable(SSTable* sstable, Memtable* memtable, void* buffer);
static off_t memtable_entry_write_sstable_index_entry(const MemtableEntry* memtable_entry, SSTableIndexEntry* index_entry, void* buffer, off_t offset);
static off_t sstable_write_index_entries(SSTable* sstable, void* buffer, off_t offset);
static off_t write_index_entry(void* buffer, SSTableIndexEntry* index_entry, off_t offset);
static off_t buffer_write(void* buffer, void* data, int size, off_t offset);
static void sstable_read_index_entries_in_memory(SSTable* sstable, uint8_t* data);

MultiMemtable* auxts_multi_memtable_create(int num_tables, int capacity) {
    MultiMemtable* multi_memtable = malloc(sizeof(MultiMemtable));
    if (!multi_memtable) {
        perror("Failed to allocate MultiMemtable");
        return NULL;
    }

    multi_memtable->index = 0;
    multi_memtable->num_tables = num_tables;
    pthread_mutex_init(&multi_memtable->mutex, NULL);

    multi_memtable->tables = malloc(num_tables * sizeof(Memtable));
    if (!multi_memtable->tables) {
        perror("Failed to allocate Memtable to MultiMemtable");
        free(multi_memtable);
        return NULL;
    }

    for (int i = 0; i < num_tables; ++i) {
        multi_memtable->tables[i] = memtable_create(capacity);
    }

    return multi_memtable;
}

void auxts_multi_memtable_append(MultiMemtable* multi_memtable, uint64_t* key, uint8_t* block, int block_size) {
    if (!multi_memtable) {
        return;
    }

    pthread_mutex_lock(&multi_memtable->mutex);

    Memtable* active_memtable = multi_memtable->tables[multi_memtable->index];
    if (!active_memtable) {
        perror("Memtable cannot be null");
        return;
    }

    memtable_append(active_memtable, key, block, block_size);
    multi_memtable->index = ++multi_memtable->index % multi_memtable->num_tables;

    pthread_mutex_unlock(&multi_memtable->mutex);
}

void auxts_multi_memtable_destroy(MultiMemtable* multi_memtable) {
    if (!multi_memtable) {
        return;
    }

    pthread_mutex_lock(&multi_memtable->mutex);

    auxts_multi_memtable_flush(multi_memtable);

    for (int i = 0; i < multi_memtable->num_tables; ++i) {
        Memtable* memtable = multi_memtable->tables[i];
        memtable_destroy(memtable);
    }

    free(multi_memtable->tables);

    pthread_mutex_unlock(&multi_memtable->mutex);
    pthread_mutex_destroy(&multi_memtable->mutex);

    free(multi_memtable);
}

void auxts_multi_memtable_flush(MultiMemtable* multi_memtable) {
    if (!multi_memtable) {
        return;
    }

    for (int i = 0; i < multi_memtable->num_tables; ++i) {
        Memtable* memtable = multi_memtable->tables[i];
        memtable_flush(memtable);
    }
}

SSTable* auxts_read_sstable_index_entries(const char* filename) {
    int fd = open(filename, O_RDONLY);
    if (fd == -1) {
        perror("Failed to open SSTable");
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
    SSTable* sstable = sstable_construct(entry_count);
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

SSTable* auxts_read_sstable_index_entries_in_memory(uint8_t* data, size_t size) {
    uint16_t num_entries;

    memcpy(&num_entries, data + (size - AUXTS_TRAILER_SIZE), sizeof(uint16_t));
    num_entries = auxts_swap16_if_big_endian(num_entries);

    size_t offset = size - (num_entries * AUXTS_INDEX_ENTRY_SIZE) - AUXTS_TRAILER_SIZE;

    SSTable* sstable = sstable_construct(num_entries);
    if (!sstable) {
        return NULL;
    }

    sstable->num_entries = num_entries;
    sstable->fd = -1;

    sstable_read_index_entries_in_memory(sstable, data + offset);

    return sstable;
}

void auxts_get_time_partitioned_path(uint64_t milliseconds, char* path) {
    struct tm info = {0};
    AUXTS__milliseconds_to_tm(milliseconds, &info);

    long remainder = (long)(milliseconds % AUXTS__MILLISECONDS_PER_SECOND);

    sprintf(path, ".data/%d/%02d/%02d/%02d/%02d/%02d/%03ld.df",
            info.tm_year + 1900, info.tm_mon + 1,
            info.tm_mday, info.tm_hour,
            info.tm_min, info.tm_sec,
            remainder);
}

void auxts_sstable_destroy_except_data(SSTable* sstable) {
    free(sstable->index_entries);
    free(sstable);
}

MemtableEntry* auxts_read_memtable_entry(uint8_t* buffer, size_t offset) {
    MemtableEntry* entry = malloc(sizeof(MemtableEntry));
    if (!entry) {
        perror("Failed to allocate MemtableEntry");
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

uint8_t* auxts_read_file_util(const char* path, long* size) {
    FILE *file = fopen(path, "rb");
    if (!file) {
        perror("Error opening file");
        return NULL;
    }

    fseek(file, 0, SEEK_END);
    *size = ftell(file);
    rewind(file);

    if (size <= 0) {
        fprintf(stderr, "Invalid file num_buckets\n");
        fclose(file);
        return NULL;
    }

    uint8_t* buffer = malloc(*size);
    if (!buffer) {
        fprintf(stderr, "Memory allocation failed\n");
        fclose(file);
        return NULL;
    }

    size_t bytes = fread(buffer, 1, *size, file);
    if (bytes != *size) {
        fprintf(stderr, "Error reading file\n");
        free(buffer);
        fclose(file);
        return NULL;
    }

    fclose(file);

    return buffer;
}

Memtable* memtable_create(int capacity) {
    Memtable* memtable = malloc(sizeof(Memtable));
    if (!memtable) {
        perror("Failed to allocate Memtable");
        return NULL;
    }

    memtable->num_entries = 0;
    memtable->capacity = capacity;

    memtable->entries = malloc(sizeof(MemtableEntry) * memtable->capacity);
    if (!memtable->entries) {
        perror("Failed to allocate MemtableEntry to Memtable");
        free(memtable);
        return NULL;
    }

    pthread_mutex_init(&memtable->mutex, NULL);

    for (int i = 0; i < memtable->capacity; ++i) {
        if (posix_memalign((void**)&memtable->entries[i].block, AUXTS__ALIGN, AUXTS_MAX_BLOCK_SIZE) != 0) {
            perror("Failed to allocate block to Memtable");
            memtable_destroy(memtable);
            return NULL;
        }
    }

    return memtable;
}

void memtable_append(Memtable* memtable, uint64_t* key, uint8_t* block, int block_size) {
    if (!memtable) {
        return;
    }

    pthread_mutex_lock(&memtable->mutex);

    if (memtable->num_entries >= memtable->capacity) {
        memtable_flush(memtable);
    }

    memcpy(memtable->entries[memtable->num_entries].key, key, sizeof(uint64_t) * 2);
    memcpy(memtable->entries[memtable->num_entries].block, block, block_size);

    memtable->entries[memtable->num_entries].block_size = block_size;
    ++memtable->num_entries;

    pthread_mutex_unlock(&memtable->mutex);
}

void memtable_flush(Memtable* memtable) {
    if (!memtable) {
        return;
    }

    int num_entries = memtable->num_entries;
    if (num_entries == 0) {
        return;
    }

    SSTable* sstable = sstable_construct(num_entries);
    if (!sstable) {
        perror("SSTable cannot be null");
        return;
    }

    sstable_write(sstable, memtable);
    auxts_sstable_destroy_except_data(sstable);

    memtable->num_entries = 0;
}

void memtable_destroy(Memtable* memtable) {
    if (!memtable) {
        return;
    }

    pthread_mutex_lock(&memtable->mutex);

    for (int i = 0; i < memtable->num_entries; ++i) {
        free(memtable->entries[i].block);
    }

    free(memtable->entries);

    pthread_mutex_unlock(&memtable->mutex);
    pthread_mutex_destroy(&memtable->mutex);

    free(memtable);
}

void sstable_read_index_entries_in_memory(SSTable* sstable, uint8_t* data) {
    off_t _offset = 0;

    for (int entry = 0; entry < sstable->num_entries; ++entry) {
        uint64_t block_id;

        memcpy(&block_id, data + _offset, sizeof(uint64_t));
        sstable->index_entries[entry].key[0] = auxts_swap64_if_big_endian(block_id);
        _offset += sizeof(uint64_t);

        uint64_t timestamp;

        memcpy(&timestamp, data + _offset, sizeof(uint64_t));
        sstable->index_entries[entry].key[1] = auxts_swap64_if_big_endian(timestamp);
        _offset += sizeof(uint64_t);

        uint64_t offset;

        memcpy(&offset, data + _offset, sizeof(uint64_t));
        sstable->index_entries[entry].offset = auxts_swap64_if_big_endian(offset);
        _offset += sizeof(uint64_t);
    }

}

void sstable_read_index_entries(SSTable* sstable) {
    for (int entry = 0; entry < sstable->num_entries; ++entry) {
        uint64_t  block_id;

        read(sstable->fd, &block_id, sizeof(uint64_t));
        sstable->index_entries[entry].key[0] = auxts_swap64_if_big_endian(block_id);

        uint64_t timestamp;

        read(sstable->fd, &timestamp, sizeof(uint64_t));
        sstable->index_entries[entry].key[1] = auxts_swap64_if_big_endian(timestamp);

        uint64_t offset;

        read(sstable->fd, &offset, sizeof(uint64_t));
        sstable->index_entries[entry].offset = auxts_swap64_if_big_endian(offset);
    }
}

void sstable_write(SSTable* sstable, Memtable* memtable) {
    if (!sstable) {
        return;
    }

    void* buffer;

    uint16_t num_entries = memtable->num_entries;
    size_t size = AUXTS_HEADER_SIZE + (num_entries * (AUXTS_MAX_BLOCK_SIZE + AUXTS_MEMTABLE_ENTRY_METADATA_SIZE + AUXTS_INDEX_ENTRY_SIZE)) + AUXTS_TRAILER_SIZE;

    if (posix_memalign(&buffer, AUXTS_BLOCK_ALIGN, size) != 0) {
        perror("Buffer allocation failed");
        return;
    }

    char path[64];

    uint64_t milliseconds = memtable->entries[0].key[1];
    create_time_partitioned_directories(milliseconds);
    auxts_get_time_partitioned_path(milliseconds, path);

    sstable->fd = open(path, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (sstable->fd == -1) {
        perror("Failed to open SSTable");
        return;
    }

    sstable->num_entries = num_entries;

    off_t offset;
    offset = sstable_write_memtable(sstable, memtable, buffer);
    offset = sstable_write_index_entries(sstable, buffer, offset);

    size_t file_size = auxts_swap64_if_big_endian(offset);

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

SSTable* sstable_construct(int num_entries) {
    SSTable* sstable = malloc(sizeof(SSTable));
    if (!sstable) {
        perror("Failed to allocate SSTable");
        return NULL;
    }

    sstable->index_entries = malloc(sizeof(SSTableIndexEntry) * num_entries);
    if (!sstable->index_entries) {
        perror("Failed to allocate SSTableIndexEntry to SSTable");
        free(sstable);
        return NULL;
    }

    return sstable;
}

off_t sstable_write_memtable(SSTable* sstable, Memtable* memtable, void* buffer) {
    off_t offset = AUXTS_HEADER_SIZE;

    for (int entry = 0; entry < sstable->num_entries; ++entry) {
        MemtableEntry* memtable_entry = &memtable->entries[entry];
        SSTableIndexEntry* index_entry = &sstable->index_entries[entry];
        offset = memtable_entry_write_sstable_index_entry(memtable_entry, index_entry, buffer, offset);
    }

    return offset;
}

// value        byte-order      bytes
// -----------------------------------------
// block-num_samples   little-endian   2
// block-id     little-endian   8
// timestamp    little-endian   8
// block        little-endian   <block-num_samples>
off_t memtable_entry_write_sstable_index_entry(const MemtableEntry* memtable_entry, SSTableIndexEntry* index_entry, void* buffer, off_t offset) {
    uint16_t block_size = auxts_swap16_if_big_endian(memtable_entry->block_size);
    index_entry->offset = offset;
    offset = buffer_write(buffer, &block_size, sizeof(uint16_t), offset);

    uint64_t block_id = auxts_swap64_if_big_endian(memtable_entry->key[0]);
    index_entry->key[0] = memtable_entry->key[0];
    offset = buffer_write(buffer, &block_id, sizeof(uint64_t), offset);

    uint64_t timestamp = auxts_swap64_if_big_endian(memtable_entry->key[1]);
    index_entry->key[1] = memtable_entry->key[1];
    offset = buffer_write(buffer, &timestamp, sizeof(uint64_t), offset);

    return buffer_write(buffer, memtable_entry->block, memtable_entry->block_size, offset);
}

off_t write_index_entry(void* buffer, SSTableIndexEntry* index_entry, off_t offset) {
    uint64_t block_id = auxts_swap64_if_big_endian(index_entry->key[0]);
    offset = buffer_write(buffer, &block_id, sizeof(uint64_t), offset);

    uint64_t timestamp = auxts_swap64_if_big_endian(index_entry->key[1]);
    offset = buffer_write(buffer, &timestamp, sizeof(uint64_t), offset);

    size_t _offset = auxts_swap64_if_big_endian(index_entry->offset);
    return buffer_write(buffer, &_offset, sizeof(size_t), offset);
}

off_t sstable_write_index_entries(SSTable* sstable, void* buffer, off_t offset) {
    for (int entry = 0; entry < sstable->num_entries; ++entry) {
        offset = write_index_entry(buffer, &sstable->index_entries[entry], offset);
    }

    uint16_t entry_count = auxts_swap16_if_big_endian(sstable->num_entries);
    return buffer_write(buffer, &entry_count, sizeof(uint16_t), offset);
}

off_t buffer_write(void* buffer, void* data, int size, off_t offset) {
    memcpy(((uint8_t*)buffer) + offset, data, size);
    return offset + size;
}


int test_multi_memtable() {
    long size1, size2, size3, size4;

    MultiMemtable* mmt = auxts_multi_memtable_create(2, 2);

    uint8_t* flac_data1 = auxts_read_file_util("../data/data-1.flac", &size1);
    uint8_t* flac_data2 = auxts_read_file_util("../data/data-2.flac", &size2);
    uint8_t* flac_data3 = auxts_read_file_util("../data/data-3.flac", &size3);
    uint8_t* flac_data4 = auxts_read_file_util("../data/data-4.flac", &size4);

    uint64_t key[2];
    AUXTS__murmur3_x64_128("test", 7, 0, key);

    printf("stream_id: %llu\n", key[0]);

    key[1] = 1739141512213;
    auxts_multi_memtable_append(mmt, key, flac_data1, size1);

    key[1] = 1739141512214;
    auxts_multi_memtable_append(mmt, key, flac_data2, size2);

    key[1] = 1739141512215;
    auxts_multi_memtable_append(mmt, key, flac_data3, size3);

    key[1] = 1739141512216;
    auxts_multi_memtable_append(mmt, key, flac_data4, size4);

    auxts_multi_memtable_destroy(mmt);

    free(flac_data1);
    free(flac_data2);
    free(flac_data3);
    free(flac_data4);

    return 0;
}
