#include "result.h"

auxts_result* auxts_result_create(size_t size) {
    auxts_result* result = malloc(sizeof(auxts_result));
    if (!result) {
        perror("Failed to allocate auxts_result");
        return NULL;
    }

    result->data = malloc(size);
    if (!result->data) {
        perror("Failed to allocate data to auxts_result");
        free(result);
        return NULL;
    }

    return result;
}

