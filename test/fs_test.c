#include "fs_test.h"


void test_fs_fname(void) {
    char *path = "/baz";

    char *fname = racs_fs_fname(path);

    TEST_ASSERT_NOT_NULL(fname);
    TEST_ASSERT_EQUAL_STRING("baz", fname);
}
