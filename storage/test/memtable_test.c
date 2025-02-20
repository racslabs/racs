#include "memtable_test.h"

void test_multi_memtable() {
    long size1, size2, size3, size4;

    MultiMemtable* mmt = auxts_multi_memtable_create(2, 2);

    uint8_t* flac_data1 = auxts_read_file_util("../test/data/data-1.flac", &size1);
    uint8_t* flac_data2 = auxts_read_file_util("../test/data/data-2.flac", &size2);
    uint8_t* flac_data3 = auxts_read_file_util("../test/data/data-3.flac", &size3);
    uint8_t* flac_data4 = auxts_read_file_util("../test/data/data-4.flac", &size4);

    uint64_t key[2];
    auxts_murmurhash3_x64_128((uint8_t*)"test", 7, 0, key);

    key[1] = 1739141512213;
    auxts_multi_memtable_append(mmt, key, flac_data1, (int)size1);

    key[1] = 1739141512214;
    auxts_multi_memtable_append(mmt, key, flac_data2, (int)size2);

    key[1] = 1739141512215;
    auxts_multi_memtable_append(mmt, key, flac_data3, (int)size3);

    key[1] = 1739141512216;
    auxts_multi_memtable_append(mmt, key, flac_data4, (int)size4);

    auxts_multi_memtable_destroy(mmt);

    free(flac_data1);
    free(flac_data2);
    free(flac_data3);
    free(flac_data4);
}