#include "extract.h"

char* AUXTS__resolve_shared_path(const char* path1, const char* path2) {
    if (!path1 || !path2) {
        perror("Paths cannot be null");
        exit(-1);
    }

    size_t len1 = strlen(path1);
    size_t len2 = strlen(path2);
    size_t len = len1 < len2 ? len1 : len2;

    size_t i = 0;
    for ( ; i < len; i++) {
        if (path1[i] != path2[i]) {
            break;
        }
    }

    if (path1[i - 1] == '/') --i;

    char* shared_path = malloc(i + 1);
    strncpy(shared_path, path1, i);
    shared_path[i] = '\0';

    return shared_path;
}

int test_extract() {
    char* path1 = "/foo/bar/baz";
    char* path2 = "/foo/bar/";

    printf("%s", AUXTS__resolve_shared_path(path1, path2));

    return 0;
}
