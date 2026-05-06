#include "path_test.h"

#include <unistd.h>


void test_path_expand(void) {
    setenv("RACS_TEST_TMP", "/tmp/racs", 1);

    char path[PATH_MAX];

    racs_path_expand(path, "$RACS_TEST_TMP/logs");
    TEST_ASSERT_EQUAL_STRING("/tmp/racs/logs", path);

    racs_path_expand(path, "~/config");
    char *home = getenv("HOME");
    TEST_ASSERT_EQUAL_INT(0, strncmp(path, home, strlen(home)));
}

void test_path_from_time(void) {
    racs_config_load("test-conf.yaml");

    char path1[PATH_MAX];
    char path2[PATH_MAX];
    char *home = getenv("HOME");

    sprintf(path1, "%s/.racs/seg/1/2024/05/04/16/00/00/123", home);
    racs_path_from_time(path2, 1ULL, 1714838400123LL);

    TEST_ASSERT_EQUAL_STRING(path1, path2);

    racs_config_destroy();
}

void test_path_from_range(void) {
    racs_config_load("test-conf.yaml");

    char path1[PATH_MAX];
    char path2[PATH_MAX];
    char *home = getenv("HOME");

    racs_time_range range = {
        .from = 1735689600000LL,
        .to   = 1735693200000LL,
    };

    sprintf(path1, "%s/.racs/seg/123/2025/01/01", home);
    racs_path_from_range(path2, 123ULL, range);

    TEST_ASSERT_EQUAL_STRING(path1, path2);

    racs_config_destroy();
}

void test_path_resolve(void) {
    char path[PATH_MAX];

    racs_path_resolve(path, "/foo/bar/baz", "/foo/tmp");
    TEST_ASSERT_EQUAL_STRING("/foo", path);
}

void test_path_mkdirs(void) {
    racs_path_mkdir("/tmp/foo/bar");

    struct stat st = {0};
    TEST_ASSERT_EQUAL_INT(0, stat("/tmp/foo", &st));

    system("rm -rf /tmp/foo");
}
