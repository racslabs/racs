#include "result.h"

void racs_result_init(racs_result *result, size_t size) {
    result->size = size;
    result->data = malloc(size);
    if (!result->data) {
        racs_log_error("Failed to allocate data to racs_result");
    }
}

void racs_result_destroy(racs_result *result) {
    free(result->data);
}

