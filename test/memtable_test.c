#include "memtable_test.h"

void test_multi_memtable() {
    int size1, size2, size3, size4;

    rats_multi_memtable* mmt = rats_multi_memtable_create(2, 2);

    uint8_t* flac_data1 = read_file("../test/data/data-1.pcm", &size1);
    uint8_t* flac_data2 = read_file("../test/data/data-2.pcm", &size2);
    uint8_t* flac_data3 = read_file("../test/data/data-3.pcm", &size3);
    uint8_t* flac_data4 = read_file("../test/data/data-4.pcm", &size4);

    uint64_t key[2];
    murmur3_x64_128((uint8_t*)"test", strlen("test"), 0, key);

    key[1] = 1739141512213;
    rats_multi_memtable_append(mmt, key, flac_data1, size1);

    key[1] = 1739141512214;
    rats_multi_memtable_append(mmt, key, flac_data2, size2);

    key[1] = 1739141512215;
    rats_multi_memtable_append(mmt, key, flac_data3, size3);

    key[1] = 1739141512216;
    rats_multi_memtable_append(mmt, key, flac_data4, size4);

    rats_multi_memtable_destroy(mmt);

    free(flac_data1);
    free(flac_data2);
    free(flac_data3);
    free(flac_data4);
}