// RACS - Remote Audio Caching Server
// Copyright (c) 2025 RACS Labs, LLC. All rights reserved.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "config.h"
#include "version.h"

static void racs_cli_help(void) {
    printf(
        "Usage: racs [options...] <file>\n"
        "-c, --config <file>     Run server with conf.yaml file\n"
        "-v, --version           Show version and quit\n"
        "-h, --help              Get help for commands\n"
    );
}

static int racs_cli_is_arg(const char *s, const char *shrt, const char *lng) {
    return strcmp(s, shrt) == 0 || strcmp(s, lng) == 0;
}

void racs_cli_args(int argc, char *argv[]) {
    if (argc < 2) {
        goto error;
    }

    const char *cmd = argv[1];

    if (argc == 2) {
        if (racs_cli_is_arg(cmd, "-v", "--version")) {
            char ver[55];
            racs_version(ver);
            printf("%s\n", ver);

            exit(0);
        }

        if (racs_cli_is_arg(cmd, "-h", "--help")) {
            racs_cli_help();
            exit(0);
        }

        if (racs_cli_is_arg(cmd, "-c", "--config")) {
            fprintf(stderr, "racs: option '-c' requires an argument\n");
            goto error;
        }
    }

    if (argc == 3 && racs_cli_is_arg(cmd, "-c", "--config")) {
        if (racs_config_load(argv[2]) == -1) {
            exit(-1);
        }
        return;
    }

    if (cmd[0] == '-') {
        fprintf(stderr, "racs: unknown option '%s'\n", cmd);
    }

    error:
        fprintf(stderr, "racs: try 'racs --help' for more information\n");
    exit(-1);
}

int main(int argc, char *argv[]) {
    racs_cli_args(argc, argv);
    return 0;
}
