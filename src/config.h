// RACS - Remote Audio Caching Server
// Copyright (c) 2025 RACS Labs, LLC. All rights reserved.
//
// Licensed under the RACS Source Available License (RACS-SAL-1.0).
// Non-commercial use only. Commercial use requires a paid license.
// Contact: sales@racslabs.com
//
// SPDX-License-Identifier: RACS-SAL-1.0

#ifndef RACS_CONFIG_H
#define RACS_CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif

#include <cyaml/cyaml.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <wordexp.h>
#include "types.h"
#include "export.h"


typedef struct {
    char *host;
    int port;
} racs_slave_config;

typedef struct {
    int tables;
    int entries;
} racs_memtable_config;

typedef struct {
    int entries;
} racs_cache_config;

typedef struct {
    int                     port;
    char*                   data_dir;
    char*                   log_dir;
    racs_memtable_config    memtable;
    racs_cache_config       cache;
    racs_slave_config*      slaves;
    racs_uint32             slaves_count;
} racs_config;

static const cyaml_schema_field_t racs_memtables_schema_fields[] = {
        CYAML_FIELD_UINT("tables", CYAML_FLAG_DEFAULT, racs_memtable_config , tables),
        CYAML_FIELD_UINT("entries", CYAML_FLAG_DEFAULT, racs_memtable_config , entries),
        CYAML_FIELD_END
};

static const cyaml_schema_field_t racs_cache_schema_fields[] = {
        CYAML_FIELD_UINT("entries", CYAML_FLAG_DEFAULT, racs_cache_config , entries),
        CYAML_FIELD_END
};

static const cyaml_schema_field_t racs_slave_schema_fields[] = {
        CYAML_FIELD_STRING_PTR("host", CYAML_FLAG_POINTER, racs_slave_config, host, 0, CYAML_UNLIMITED),
        CYAML_FIELD_UINT("port", CYAML_FLAG_DEFAULT, racs_slave_config, port),
        CYAML_FIELD_END
};

static const cyaml_schema_value_t racs_slaves_entry_mapping = {
        CYAML_VALUE_MAPPING(CYAML_FLAG_DEFAULT, racs_slave_config, racs_slave_schema_fields),
};

static const cyaml_schema_field_t racs_schema_fields[] = {
        CYAML_FIELD_UINT("port", CYAML_FLAG_DEFAULT, racs_config , port),
        CYAML_FIELD_STRING_PTR("data_dir", CYAML_FLAG_POINTER, racs_config, data_dir, 0, CYAML_UNLIMITED),
        CYAML_FIELD_MAPPING("memtable", CYAML_FLAG_DEFAULT, racs_config, memtable, racs_memtables_schema_fields),
        CYAML_FIELD_MAPPING("cache", CYAML_FLAG_DEFAULT, racs_config , cache, racs_cache_schema_fields),
        CYAML_FIELD_STRING_PTR("log_dir", CYAML_FLAG_DEFAULT, racs_config , log_dir, 0, CYAML_UNLIMITED),
        CYAML_FIELD_SEQUENCE_COUNT(
           "slaves",
           CYAML_FLAG_OPTIONAL | CYAML_FLAG_POINTER,
           racs_config,
           slaves,
           slaves_count,
           &racs_slaves_entry_mapping,
           0,
           CYAML_UNLIMITED
       ),
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

RACS_FORCE_INLINE char *racs_expand_path(const char* path) {
    wordexp_t p;
    if (wordexp(path, &p, 0) != 0) {
        printf("racs: could not expand path: %s", path);
        exit(-1);
    }

    char *result = strdup(p.we_wordv[0]);
    wordfree(&p);
    return result;
}

RACS_FORCE_INLINE void racs_config_load(racs_config** config, const char* path) {
    cyaml_err_t err = cyaml_load_file(path, &yaml_config, &racs_schema, (void **)config, NULL);
    if (err != CYAML_OK) {
        fprintf(stderr, "CYAML error: %s\n", cyaml_strerror(err));
        exit(1);
    }

    char *exp_data_dir = racs_expand_path(config[0]->data_dir);
    char *exp_log_dir = racs_expand_path(config[0]->log_dir);

    free(config[0]->data_dir);
    free(config[0]->log_dir);

    config[0]->data_dir = exp_data_dir;
    config[0]->log_dir = exp_log_dir;
}

RACS_FORCE_INLINE void racs_config_destroy(racs_config* config) {
    cyaml_free(&yaml_config, &racs_schema, config, 0);
}

#ifdef __cplusplus
}
#endif

#endif //RACS_CONFIG_H
