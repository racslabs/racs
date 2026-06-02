#include "fs_test.h"

void print_cb(const char *path, void *data);

void print_cb(const char *path, void *data) {
    (void)data;
    printf("%s\n", path);
}


void test_fs_fname(void) {
    char *path = "/baz";

    char *fname = racs_fs_fname(path);

    TEST_ASSERT_NOT_NULL(fname);
    TEST_ASSERT_EQUAL_STRING("baz", fname);

    racs_fs_walk("/home/corbin/.racs/seg", print_cb, NULL);
}
