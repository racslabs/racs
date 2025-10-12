// RACS - Remote Audio Caching Server
// Copyright (c) 2025 RACS Labs, LLC. All rights reserved.
//
// Licensed under the RACS Source Available License (RSAL-1.0).
// Non-commercial use only. Commercial use requires a paid license.
// Contact: sales@racslabs.com
//
// SPDX-License-Identifier: RACS-SAL-1.0

#ifndef RACS_RESULT_H
#define RACS_RESULT_H

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include "types.h"
#include "log.h"

typedef struct {
    racs_uint8 *data;
    size_t size;
} racs_result;

void racs_result_init(racs_result *result, size_t size);

void racs_result_destroy(racs_result *result);


#endif //RACS_RESULT_H
