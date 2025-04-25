#include "result.h"

void rats_result_init(rats_result* result, size_t size) {
    result->size = size;
    result->data = malloc(size);
    if (!result->data) {
        perror("Failed to allocate data to rats_result");
    }
}

void rats_result_destroy(rats_result* result) {
    free(result->data);
}

