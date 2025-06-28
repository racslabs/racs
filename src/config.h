
#ifndef RACS_CONFIG_H
#define RACS_CONFIG_H

#include <cyaml/cyaml.h>
#include <stdio.h>
#include <stdlib.h>
#include "types.h"
#include "export.h"

typedef struct {
    int tables;
    int entries;
} racs_memtable_config;

typedef struct {
    int entries;
} racs_cache_config;

typedef struct {
    int size;
    char* log_dir;
} racs_log_config;

typedef struct {
    int                     port;
    char*                   data_dir;
    racs_memtable_config    memtable;
    racs_cache_config       cache;
    racs_log_config         log;
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

static const cyaml_schema_field_t racs_log_schema_fields[] = {
        CYAML_FIELD_UINT("size", CYAML_FLAG_DEFAULT, racs_log_config , size),
        CYAML_FIELD_STRING_PTR("log_dir", CYAML_FLAG_POINTER, racs_log_config, log_dir, 0, CYAML_UNLIMITED),
        CYAML_FIELD_END
};

static const cyaml_schema_field_t racs_schema_fields[] = {
        CYAML_FIELD_UINT("port", CYAML_FLAG_DEFAULT, racs_config , port),
        CYAML_FIELD_STRING_PTR("data_dir", CYAML_FLAG_POINTER, racs_config, data_dir, 0, CYAML_UNLIMITED),
        CYAML_FIELD_MAPPING("memtable", CYAML_FLAG_DEFAULT, racs_config, memtable, racs_memtables_schema_fields),
        CYAML_FIELD_MAPPING("cache", CYAML_FLAG_DEFAULT, racs_config , cache, racs_cache_schema_fields),
        CYAML_FIELD_MAPPING("log", CYAML_FLAG_DEFAULT, racs_config , log, racs_log_schema_fields),
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

RACS_FORCE_INLINE void racs_config_load(racs_config** config, const char* path) {
    cyaml_err_t err = cyaml_load_file(path, &yaml_config, &racs_schema, (void **)config, NULL);
    if (err != CYAML_OK) {
        fprintf(stderr, "CYAML error: %s\n", cyaml_strerror(err));
        exit(1);
    }
}

RACS_FORCE_INLINE void racs_config_destroy(racs_config* config) {
    cyaml_free(&yaml_config, &racs_schema, config, 0);
}

#endif //RACS_CONFIG_H
