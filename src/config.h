
#ifndef RATS_CONFIG_H
#define RATS_CONFIG_H

#include <cyaml/cyaml.h>
#include <stdio.h>
#include <stdlib.h>
#include "types.h"
#include "export.h"

typedef struct {
    int count;
    int capacity;
} rats_memtable_config;

typedef struct {
    int entries;
} rats_cache_config;

typedef struct {
    int                     port;
    char*                   host;
    char*                   data_dir;
    rats_memtable_config    memtables;
    rats_cache_config       cache;
} rats_config;

static const cyaml_schema_field_t rats_memtables_schema_fields[] = {
        CYAML_FIELD_UINT("count", CYAML_FLAG_DEFAULT, rats_memtable_config , count),
        CYAML_FIELD_UINT("capacity", CYAML_FLAG_DEFAULT, rats_memtable_config , capacity),
        CYAML_FIELD_END
};

static const cyaml_schema_field_t rats_cache_schema_fields[] = {
        CYAML_FIELD_UINT("entries", CYAML_FLAG_DEFAULT, rats_cache_config , entries),
        CYAML_FIELD_END
};

static const cyaml_schema_field_t rats_schema_fields[] = {
        CYAML_FIELD_STRING_PTR("host", CYAML_FLAG_POINTER, rats_config , host, 0, CYAML_UNLIMITED),
        CYAML_FIELD_UINT("port", CYAML_FLAG_DEFAULT, rats_config , port),
        CYAML_FIELD_STRING_PTR("data_dir", CYAML_FLAG_POINTER, rats_config, data_dir, 0, CYAML_UNLIMITED),
        CYAML_FIELD_MAPPING("memtables", CYAML_FLAG_DEFAULT, rats_config, memtables, rats_memtables_schema_fields),
        CYAML_FIELD_MAPPING("cache", CYAML_FLAG_DEFAULT, rats_config , cache, rats_cache_schema_fields),
        CYAML_FIELD_END
};

static const cyaml_schema_value_t rats_schema = {
        CYAML_VALUE_MAPPING(CYAML_FLAG_POINTER, rats_config, rats_schema_fields)
};

const cyaml_config_t yaml_config = {
        .log_fn = cyaml_log, // or NULL
        .mem_fn = cyaml_mem, // or NULL
        .flags = CYAML_CFG_DEFAULT,
        .log_level = CYAML_LOG_WARNING,
};

RATS_FORCE_INLINE void rats_config_load(rats_config** config, const char* path) {
    cyaml_err_t err = cyaml_load_file(path, &yaml_config, &rats_schema, (void **)config, NULL);
    if (err != CYAML_OK) {
        fprintf(stderr, "CYAML error: %s\n", cyaml_strerror(err));
        exit(EXIT_FAILURE);
    }
}

RATS_FORCE_INLINE void rats_config_destroy(rats_config* config) {
    cyaml_free(&yaml_config, &rats_schema, config, 0);
}


#endif //RATS_CONFIG_H
