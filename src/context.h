// RACS - Remote Audio Caching Server
// Copyright (c) 2025 RACS Labs, LLC. All rights reserved.
//
// Licensed under the RACS Source Available License (RSAL-1.0).
// Non-commercial use only. Commercial use requires a paid license.
// Contact: sales@racslabs.com
//
// SPDX-License-Identifier: RACS-SAL-1.0

#ifndef RACS_CONTEXT_H
#define RACS_CONTEXT_H

#include "config.h"
#include "cache.h"
#include "stream.h"
#include "memtable.h"
#include "log.h"

typedef struct {
    racs_config *config;
    racs_cache *scache;
    racs_cache *mcache;
    racs_streamkv *kv;
    racs_multi_memtable *mmt;
} racs_context;

void racs_context_init(racs_context *ctx, const char *path);

void racs_context_destroy(racs_context *ctx);

#endif //RACS_CONTEXT_H
