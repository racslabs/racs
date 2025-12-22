// RACS - Remote Audio Caching Server
// Copyright (c) 2025 RACS Labs, LLC. All rights reserved.
//
// Licensed under the RACS Source Available License (RACS-SAL-1.0).
// Non-commercial use only. Commercial use requires a paid license.
// Contact: sales@racslabs.com
//
// SPDX-License-Identifier: RACS-SAL-1.0

#ifndef RACS_CONTEXT_H
#define RACS_CONTEXT_H

#ifdef __cplusplus
extern "C" {
#endif

#include "config.h"
#include "cache.h"
#include "stream.h"
#include "memtable.h"
#include "offsets.h"
#include "log.h"

typedef struct {
    racs_config *config;
    racs_cache *scache;
    racs_offsets *offsets;
    racs_streamkv *kv;
    racs_multi_memtable *mmt;
} racs_context;

void racs_context_init(racs_context *ctx, const char *path);

void racs_context_destroy(racs_context *ctx);

#ifdef __cplusplus
}
#endif

#endif //RACS_CONTEXT_H
