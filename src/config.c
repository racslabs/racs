// RACS - Remote Audio Caching Server
// Copyright (c) 2025 RACS Labs, LLC. All rights reserved.
//
// Licensed under the RACS Source Available License (RACS-SAL-1.0).
// Non-commercial use only. Commercial use requires a paid license.
// Contact: sales@racslabs.com
//

#include "config.h"


static racs_config *config = NULL;


int racs_config_load(const char* path) {
    if (config) {
        racs_config_destroy();
    }

    cyaml_err_t err = cyaml_load_file(path, &yaml_config, &racs_schema, (void **)&config, NULL);
    if (err != CYAML_OK) {
        fprintf(stderr, "racs: failed to load config: %s\n", cyaml_strerror(err));
        return -1;
    }

    return 0;
}

racs_config *racs_config_get(void) {
    return config;
}

void racs_config_destroy(void) {
    if (config) {
        cyaml_free(&yaml_config, &racs_schema, config, 0);
        config = NULL;
    }
}
