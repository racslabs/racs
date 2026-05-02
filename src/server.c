// RACS - Remote Audio Caching Server
// Copyright (c) 2025 RACS Labs, LLC. All rights reserved.
//
// Licensed under the RACS Source Available License (RACS-SAL-1.0).
// Non-commercial use only. Commercial use requires a paid license.
// Contact: sales@racslabs.com
//

#include "server.h"

static int racs_is_version(const char *s);

static int racs_is_help(const char *s);

static int racs_is_config(const char *s);

static int racs_is_option(const char *s);


int racs_is_version(const char *s) {
    return strcmp(s, "--version") == 0 || strcmp(s, "-v") == 0;
}

int racs_is_help(const char *s) {
    return strcmp(s, "--help") == 0 || strcmp(s, "-h") == 0;
}

int racs_is_config(const char *s) {
    return strcmp(s, "--config") == 0 || strcmp(s, "-c") == 0;
}

int racs_is_option(const char *s) {
    return strncmp(s, "-", 1) == 0;
}

void racs_help() {
    printf("Usage: racs [options...] <file>\n");
    printf("-c, --config <file>     Run server with conf.yaml file\n");
    printf("-v, --version           Show version and quit\n");
    printf("-h, --help              Get help for commands\n");
}

void racs_args(int argc, char *argv[]) {
    if (argc < 2 || argc > 3) {
        printf("racs: try 'racs --help' for more information\n");
        exit(-1);
    }

    const char *arg = argv[1];

    if (argc == 2) {
        if (racs_is_version(arg)) {
            char ver[55];
            racs_version(ver);
            printf("%s\n", ver);
            exit(0);
        }

        if (racs_is_help(arg)) {
            racs_help();
            exit(0);
        }

        if (racs_is_config(arg)) {
            printf("racs: required arg <file> is missing\n");
        } else if (racs_is_option(arg)) {
            printf("racs: option %s: is unknown\n", arg);
        }

        printf("racs: try 'racs --help' for more information\n");
        exit(-1);
    }

    if (argc == 3) {
        if (racs_is_version(arg) || racs_is_help(arg)) {
            printf("racs: try 'racs --help' for more information\n");
            exit(-1);
        }

        if (racs_is_config(arg)) {
            return;
        }

        if (racs_is_option(arg)) {
            printf("racs: option %s: is unknown\n", arg);
        }

        printf("racs: try 'racs --help' for more information\n");
        exit(-1);
    }
}

int main(int argc, char *argv[]) {
    racs_args(argc, argv);
    return 0;
}