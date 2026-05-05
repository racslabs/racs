#include "time_test.h"


void test_time_from_ts(void) {
    struct timespec ts;

    ts.tv_sec = 1;
    ts.tv_nsec = 500000000;
    TEST_ASSERT_EQUAL_INT64(1500, racs_time_from_ts(&ts));

    ts.tv_sec = 10;
    ts.tv_nsec = 999999;
    TEST_ASSERT_EQUAL_INT64(10000, racs_time_from_ts(&ts));
}

void test_time_to_tm(void) {
    struct tm info;

    racs_time timestamp = 1714838401000LL;
    racs_time_to_tm(&info, timestamp);

    TEST_ASSERT_EQUAL_INT(2024 - 1900, info.tm_year);
    TEST_ASSERT_EQUAL_INT(4, info.tm_mon);
    TEST_ASSERT_EQUAL_INT(4, info.tm_mday);
    TEST_ASSERT_EQUAL_INT(16, info.tm_hour);
    TEST_ASSERT_EQUAL_INT(0, info.tm_min);
    TEST_ASSERT_EQUAL_INT(1, info.tm_sec);
    TEST_ASSERT_EQUAL_INT(0, info.tm_isdst);
}

void test_time_to_rfc3339(void) {
    char buf[55];

    racs_time timestamp = 1714838400123LL;
    racs_time_to_rfc3339(buf, timestamp);

    TEST_ASSERT_EQUAL_STRING("2024-05-04T16:00:00.123Z", buf);
}

void test_time_from_path(void) {
    char *path = ".racs/seg/1/2024/05/04/16/00/00/123";
    racs_time timestamp = racs_time_from_path(path);

    TEST_ASSERT_EQUAL_INT64(1714838400123LL, timestamp);
}
