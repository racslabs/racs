
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
#include "rfc.h"
#include "murmur3.h"

#define AUXTS_BLOCK_ALIGN 4096

#define AUXTS_MAX_NUM_MEMTABLES 2

#define AUXTS_MAX_BLOCK_SIZE 65536

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
    uint16_t entry_count;
    AUXTS__SSTableIndexEntry* index_entries;
} AUXTS__SSTable;

typedef struct {
    int index;
    AUXTS__Memtable* tables[AUXTS_MAX_NUM_MEMTABLES];
    pthread_mutex_t mutex;
} AUXTS__MultiMemtable;

AUXTS__MultiMemtable* AUXTS__MultiMemtable_construct(int memtable_capacity);
void AUXTS__MultiMemtable_append(AUXTS__MultiMemtable* multi_memtable, uint64_t* key, uint8_t* block, int block_size);
void AUXTS__MultiMemtable_destroy(AUXTS__MultiMemtable* multi_memtable);
void AUXTS__MultiMemtable_flush(AUXTS__MultiMemtable* multi_memtable);
AUXTS__SSTable* AUXTS__read_sstable(const char* filename);
void AUXTS__SSTable_destroy(AUXTS__SSTable* sstable);

int test_multi_memtable();

#endif //AUXTS_MEMTABLE_H
