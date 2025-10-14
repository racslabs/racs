// RACS - Remote Audio Caching Server
// Copyright (c) 2025 RACS Labs, LLC. All rights reserved.
//
// Licensed under the RACS Source Available License (RACS-SAL-1.0).
// Non-commercial use only. Commercial use requires a paid license.
// Contact: sales@racslabs.com
//
// SPDX-License-Identifier: RACS-SAL-1.0

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

