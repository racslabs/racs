#include "server.h"


static int is_version(const char *s) {
    return strcmp(s, "--version") == 0 || strcmp(s, "-v") == 0;
}

static int is_help(const char *s) {
    return strcmp(s, "--help") == 0 || strcmp(s, "-h") == 0;
}

static int is_config(const char *s) {
    return strcmp(s, "--config") == 0 || strcmp(s, "-c") == 0;
}

static int is_option(const char *s) {
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
        if (is_version(arg)) {
            char ver[55];
            racs_version(ver);
            printf("%s\n", ver);
            exit(0);
        }

        if (is_help(arg)) {
            racs_help();
            exit(0);
        }

        if (is_config(arg))
            printf("racs: required arg <file> is missing\n");
        else if (is_option(arg))
            printf("racs: option %s: is unknown\n", arg);

        printf("racs: try 'racs --help' for more information\n");
        exit(-1);
    }

    if (argc == 3) {
        if (is_version(arg) || is_help(arg)) {
            printf("racs: try 'racs --help' for more information\n");
            exit(-1);
        }

        if (is_config(arg))
            return;

        if (is_option(arg))
            printf("racs: option %s: is unknown\n", arg);


        printf("racs: try 'racs --help' for more information\n");
        exit(-1);
    }
}

int main(int argc, char *argv[]) {
    racs_args(argc, argv);

    return 0;
}