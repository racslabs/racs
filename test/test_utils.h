
#ifndef RATS_TEST_UTILS_H
#define RATS_TEST_UTILS_H

#include <msgpack.h>
#include "../src/export.h"

RATS_FORCE_INLINE uint8_t* read_file(const char* path, int* size) {
    FILE *file = fopen(path, "rb");
    if (!file) {
        perror("Error opening file");
        return NULL;
    }

    fseek(file, 0, SEEK_END);
    *size = ftell(file);
    rewind(file);

    if (size <= 0) {
        fprintf(stderr, "Invalid file entries\n");
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

RATS_FORCE_INLINE void msgpack_str_assert(const char* expected, const msgpack_object_str* obj_str) {
    char buf[1024];
    strncpy(buf, obj_str->ptr, obj_str->size);
    buf[obj_str->size] = '\0';
    assert(strcmp(buf, expected) == 0);
}

#endif //RATS_TEST_UTILS_H
