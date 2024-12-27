
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
} MemtableEntry;

typedef struct {
    MemtableEntry* entries;
    uint16_t size;
    uint16_t capacity;
    pthread_mutex_t mutex;
} Memtable;

typedef struct {
    uint64_t key[2];
    size_t offset;
} SSTableIndexEntry;

typedef struct {
    int fd;
    uint16_t entry_count;
    SSTableIndexEntry* index_entries;
} SSTable;

typedef struct {
    int index;
    Memtable* tables[AUXTS_MAX_NUM_MEMTABLES];
    pthread_mutex_t mutex;
} MultiMemtable;

MultiMemtable* create_multi_memtable(int memtable_capacity);

void append_to_multi_memtable(MultiMemtable* multi_memtable, uint64_t* key, uint8_t* block, int block_size);

void destroy_multi_memtable(MultiMemtable* multi_memtable);

void flush_multi_memtable(MultiMemtable* multi_memtable);

Memtable* create_memtable(int capacity);

void append_to_memtable(Memtable* memtable, uint64_t* key, uint8_t* block, int block_size);

void flush_memtable(Memtable* memtable);

void destroy_memtable(Memtable* memtable);

SSTable* create_sstable(int entry_count);

SSTable* read_sstable(const char* filename);

void destroy_sstable(SSTable* sstable);

void create_time_partitioned_directories(uint64_t milliseconds);

void get_time_partitioned_path(uint64_t milliseconds, char* path);

void read_index_entries(SSTable* sstable);

void write_to_sstable(SSTable* sstable, Memtable* memtable);

off_t write_memtable_entries_to_sstable(void* buffer, SSTable* sstable, Memtable* memtable);

off_t write_memtable_entry(void* buffer, off_t offset, const MemtableEntry* memtable_entry, SSTableIndexEntry* index_entry);

off_t write_index_entries_to_sstable(void* buffer, SSTable* sstable, off_t offset);

off_t write_index_entry(void* buffer, SSTableIndexEntry* index_entry, off_t offset);

off_t write_to_buffer(void* buffer, void* data, int size, off_t offset);

int test_multi_memtable();

#endif //AUXTS_MEMTABLE_H
