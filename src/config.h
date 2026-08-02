// RACS - Remote Audio Caching Server
// Copyright (c) 2025 RACS Labs, LLC. All rights reserved.
//
// Licensed under the RACS Source Available License (RACS-SAL-1.0).
// Non-commercial use only. Commercial use requires a paid license.
// Contact: sales@racslabs.com
//

#ifndef RACS_CONFIG_H
#define RACS_CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif

#include <cyaml/cyaml.h>
#include "path.h"


typedef struct {
    racs_uint32 tables;
    racs_uint32 entries;
    racs_uint32 samples_per_block;
} racs_memtable_config;

typedef struct {
    racs_uint32 entries;
} racs_cache_config;

typedef struct {
    racs_uint32 fsync;
} racs_wal_config;

typedef struct {
    racs_uint32 port;
    char *data_dir;
    char *log_dir;
    racs_memtable_config memtable;
    racs_cache_config cache;
    racs_wal_config wal;
} racs_config;


static const cyaml_schema_field_t racs_memtables_schema_fields[] = {
    CYAML_FIELD_UINT("tables", CYAML_FLAG_DEFAULT, racs_memtable_config, tables),
    CYAML_FIELD_UINT("entries", CYAML_FLAG_DEFAULT, racs_memtable_config, entries),
    CYAML_FIELD_UINT("samples_per_block", CYAML_FLAG_DEFAULT, racs_memtable_config, samples_per_block),
    CYAML_FIELD_END
};

static const cyaml_schema_field_t racs_cache_schema_fields[] = {
    CYAML_FIELD_UINT("entries", CYAML_FLAG_DEFAULT, racs_cache_config, entries),
    CYAML_FIELD_END
};

static const cyaml_schema_field_t racs_wal_schema_fields[] = {
    CYAML_FIELD_UINT("fsync", CYAML_FLAG_DEFAULT, racs_wal_config, fsync),
    CYAML_FIELD_END
};

static const cyaml_schema_field_t racs_schema_fields[] = {
    CYAML_FIELD_UINT("port", CYAML_FLAG_DEFAULT, racs_config, port),
    CYAML_FIELD_STRING_PTR("data_dir", CYAML_FLAG_POINTER, racs_config, data_dir, 0, CYAML_UNLIMITED),
    CYAML_FIELD_MAPPING("memtable", CYAML_FLAG_DEFAULT, racs_config, memtable, racs_memtables_schema_fields),
    CYAML_FIELD_MAPPING("cache", CYAML_FLAG_DEFAULT, racs_config, cache, racs_cache_schema_fields),
    CYAML_FIELD_MAPPING("wal", CYAML_FLAG_DEFAULT, racs_config, wal, racs_wal_schema_fields),
    CYAML_FIELD_STRING_PTR("log_dir", CYAML_FLAG_DEFAULT, racs_config, log_dir, 0, CYAML_UNLIMITED),
    CYAML_FIELD_END
};

static const cyaml_schema_value_t racs_schema = {
    CYAML_VALUE_MAPPING(CYAML_FLAG_POINTER, racs_config, racs_schema_fields)
};

static const cyaml_config_t yaml_config = {
    .log_fn = cyaml_log,
    .mem_fn = cyaml_mem,
    .flags = CYAML_CFG_DEFAULT,
    .log_level = CYAML_LOG_WARNING,
};

int racs_config_load(const char *path);

racs_config *racs_config_get(void);

void racs_config_destroy(void);


#ifdef __cplusplus
}
#endif

#endif //RACS_CONFIG_H
