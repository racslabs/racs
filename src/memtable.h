
#ifndef AUXTS_MEMTABLE_H
#define AUXTS_MEMTABLE_H

#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <errno.h>
#include <pthread.h>
#include "bytes.h"
#include "time.h"
#include "murmur3.h"

#ifdef __cplusplus
extern "C" {
#endif

#define AUXTS_BLOCK_ALIGN 4096

#define AUXTS_MAX_BLOCK_SIZE 65536

#define AUXTS_INDEX_ENTRY_SIZE 24

#define AUXTS_HEADER_SIZE 8

#define AUXTS_MEMTABLE_ENTRY_METADATA_SIZE 18

#define AUXTS_TRAILER_SIZE 2

typedef struct {
    uint64_t key[2];
    uint8_t* block;
    uint16_t block_size;
} rats_memtable_entry;

typedef struct {
    rats_memtable_entry* entries;
    uint16_t num_entries;
    uint16_t capacity;
    pthread_mutex_t mutex;
} rats_memtable;

typedef struct {
    uint64_t key[2];
    size_t offset;
} rats_sstable_index_entry;

typedef struct {
    int fd;
    size_t size;
    uint8_t* data;
    uint16_t num_entries;
    rats_sstable_index_entry* index_entries;
} rats_sstable;

typedef struct {
    int index;
    int num_tables;
    rats_memtable** tables;
    pthread_mutex_t mutex;
} rats_multi_memtable;

rats_multi_memtable* rats_multi_memtable_create(int num_tables, int capacity);
void rats_multi_memtable_append(rats_multi_memtable* mmt, uint64_t* key, uint8_t* block, uint16_t block_size);
void rats_multi_memtable_destroy(rats_multi_memtable* mmt);
void rats_multi_memtable_flush(rats_multi_memtable* mmt);
rats_sstable* rats_sstable_read(const char* filename);
rats_sstable* rats_sstable_read_in_memory(uint8_t* data, size_t size);
void rats_sstable_destroy_except_data(rats_sstable* sst);
rats_memtable_entry* rats_memtable_entry_read(uint8_t* buf, size_t offset);

#ifdef __cplusplus
}
#endif

#endif //AUXTS_MEMTABLE_H
