
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
#include "endian.h"
#include "timestamp.h"
#include "murmurhash3.h"

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
} auxts_memtable_entry;

typedef struct {
    auxts_memtable_entry* entries;
    uint16_t num_entries;
    uint16_t capacity;
    pthread_mutex_t mutex;
} auxts_memtable;

typedef struct {
    uint64_t key[2];
    size_t offset;
} auxts_sstable_index_entry;

typedef struct {
    int fd;
    size_t size;
    uint8_t* data;
    uint16_t num_entries;
    auxts_sstable_index_entry* index_entries;
} auxts_sstable;

typedef struct {
    int index;
    int num_tables;
    auxts_memtable** tables;
    pthread_mutex_t mutex;
} auxts_multi_memtable;

auxts_multi_memtable* auxts_multi_memtable_create(int num_tables, int capacity);
void auxts_multi_memtable_append(auxts_multi_memtable* mmt, uint64_t* key, uint8_t* block, int block_size);
void auxts_multi_memtable_destroy(auxts_multi_memtable* mmt);
void auxts_multi_memtable_flush(auxts_multi_memtable* mmt);
auxts_sstable* auxts_read_sstable_index_entries(const char* filename);
auxts_sstable* auxts_read_sstable_index_entries_in_memory(uint8_t* data, size_t size);
void auxts_sstable_destroy_except_data(auxts_sstable* sst);
void auxts_get_time_partitioned_path(uint64_t milliseconds, char* path);
auxts_memtable_entry* auxts_read_memtable_entry(uint8_t* buffer, size_t offset);

#ifdef __cplusplus
}
#endif

#endif //AUXTS_MEMTABLE_H
