#include "result.h"

auxts_result_t* auxts_result_create(size_t size) {
    auxts_result_t* result = malloc(sizeof(auxts_result_t));
    if (!result) {
        perror("Failed to allocate auxts_result_t");
        return NULL;
    }

    result->data = malloc(size);
    if (!result->data) {
        perror("Failed to allocate data to auxts_result_t");
        free(result);
        return NULL;
    }

    return result;
}

