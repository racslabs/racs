#include "result.h"

void auxts_result_init(auxts_result* result, size_t size) {
    result->size = size;
    result->data = malloc(size);
    if (!result->data) {
        perror("Failed to allocate data to auxts_result");
    }
}

void auxts_result_destroy(auxts_result* result) {
    free(result->data);
}

