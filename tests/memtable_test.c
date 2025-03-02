#include "memtable_test.h"

uint8_t* read_file_test_util(const char* path, int* size) {
    FILE *file = fopen(path, "rb");
    if (!file) {
        perror("Error opening file");
        return NULL;
    }

    fseek(file, 0, SEEK_END);
    *size = ftell(file);
    rewind(file);

    if (size <= 0) {
        fprintf(stderr, "Invalid file capacity\n");
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

void test_multi_memtable() {
    int size1, size2, size3, size4;

    auxts_multi_memtable* mmt = auxts_multi_memtable_create(2, 2);

    uint8_t* flac_data1 = read_file_test_util("../tests/data/data-1.flac", &size1);
    uint8_t* flac_data2 = read_file_test_util("../tests/data/data-2.flac", &size2);
    uint8_t* flac_data3 = read_file_test_util("../tests/data/data-3.flac", &size3);
    uint8_t* flac_data4 = read_file_test_util("../tests/data/data-4.flac", &size4);

    uint64_t key[2];
    auxts_mmh3_x64_128((uint8_t *) "tests", 4, 0, key);

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
}