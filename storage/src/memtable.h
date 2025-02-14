
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
#include "murmur3.h"

#ifdef __cplusplus
extern "C" {
#endif

#define AUXTS__MAX_NUM_MEMTABLES 2

extern AUXTS_API const int AUXTS__BLOCK_ALIGN;

extern AUXTS_API const int AUXTS__MAX_BLOCK_SIZE;

extern AUXTS_API const int AUXTS__INDEX_ENTRY_SIZE;

extern AUXTS_API const int AUXTS__HEADER_SIZE;

extern AUXTS_API const int AUXTS__MEMTABLE_ENTRY_METADATA_SIZE;

extern AUXTS_API const int AUXTS__TRAILER_SIZE;

typedef struct {
    uint64_t key[2];
    uint8_t* block;
    uint16_t block_size;
} AUXTS__MemtableEntry;

typedef struct {
    AUXTS__MemtableEntry* entries;
    uint16_t size;
    uint16_t capacity;
    pthread_mutex_t mutex;
} AUXTS__Memtable;

typedef struct {
    uint64_t key[2];
    size_t offset;
} AUXTS__SSTableIndexEntry;

typedef struct {
    int fd;
    size_t size;
    uint16_t entry_count;
    uint8_t* buffer;
    AUXTS__SSTableIndexEntry* index_entries;
} AUXTS__SSTable;

typedef struct {
    int index;
    AUXTS__Memtable* tables[AUXTS__MAX_NUM_MEMTABLES];
    pthread_mutex_t mutex;
} AUXTS__MultiMemtable;

AUXTS_API AUXTS__MultiMemtable* AUXTS__MultiMemtable_construct(int capacity);
AUXTS_API void AUXTS__MultiMemtable_append(AUXTS__MultiMemtable* multi_memtable, uint64_t* key, uint8_t* block, int block_size);
AUXTS_API void AUXTS__MultiMemtable_destroy(AUXTS__MultiMemtable* multi_memtable);
AUXTS_API void AUXTS__MultiMemtable_flush(AUXTS__MultiMemtable* multi_memtable);
AUXTS_API AUXTS__SSTable* AUXTS__read_sstable_index_entries(const char* filename);
AUXTS_API AUXTS__SSTable* AUXTS__read_sstable_index_entries_in_memory(uint8_t* buffer, size_t size);
AUXTS_API void AUXTS__SSTable_destroy(AUXTS__SSTable* sstable);
AUXTS_API void AUXTS__get_time_partitioned_path(uint64_t milliseconds, char* path);
AUXTS_API AUXTS__MemtableEntry* AUXTS__read_memtable_entry(uint8_t* buffer, size_t offset);

int test_multi_memtable();

#ifdef __cplusplus
}
#endif

#endif //AUXTS_MEMTABLE_H
