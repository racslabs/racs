
#ifndef RATS_MEMTABLE_H
#define RATS_MEMTABLE_H

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

#define RATS_BLOCK_ALIGN 4096

#define RATS_MAX_BLOCK_SIZE 65536

#define RATS_INDEX_ENTRY_SIZE 24

#define RATS_HEADER_SIZE 8

#define RATS_MEMTABLE_ENTRY_METADATA_SIZE 18

#define RATS_TRAILER_SIZE 2

typedef struct {
    rats_uint64 key[2];
    rats_uint8* block;
    rats_uint16 block_size;
} rats_memtable_entry;

typedef struct {
    rats_memtable_entry* entries;
    rats_uint16 num_entries;
    rats_uint16 capacity;
    pthread_mutex_t mutex;
} rats_memtable;

typedef struct {
    rats_uint64 key[2];
    size_t offset;
} rats_sstable_index_entry;

typedef struct {
    int fd;
    size_t size;
    rats_uint8* data;
    rats_uint16 num_entries;
    rats_sstable_index_entry* index_entries;
} rats_sstable;

typedef struct {
    int index;
    int num_tables;
    rats_memtable** tables;
    pthread_mutex_t mutex;
} rats_multi_memtable;

rats_multi_memtable* rats_multi_memtable_create(int num_tables, int capacity);
void rats_multi_memtable_append(rats_multi_memtable* mmt, rats_uint64* key, rats_uint8* block, rats_uint16 block_size);
void rats_multi_memtable_destroy(rats_multi_memtable* mmt);
void rats_multi_memtable_flush(rats_multi_memtable* mmt);
rats_sstable* rats_sstable_read(const char* filename);
rats_sstable* rats_sstable_read_in_memory(rats_uint8* data, size_t size);
void rats_sstable_destroy_except_data(rats_sstable* sst);
rats_memtable_entry* rats_memtable_entry_read(rats_uint8* buf, size_t offset);

#ifdef __cplusplus
}
#endif

#endif //RATS_MEMTABLE_H
