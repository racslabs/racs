
#ifndef AUXTS_MEMTABLE_TEST_H
#define AUXTS_MEMTABLE_TEST_H

#include <assert.h>
#include "../src/memtable.h"

uint8_t* read_file_test_util(const char* path, int* size);
void test_multi_memtable();

#endif //AUXTS_MEMTABLE_TEST_H
