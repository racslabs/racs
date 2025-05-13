#include "timestamp_test.h"

void test_rfc3339() {
    uint64_t milliseconds = 1734272943291;

    char buf[55];
    racs_time_to_rfc3339(milliseconds, buf);

    assert(strcmp("2024-12-15T14:29:03.291Z", buf) == 0);
    assert(racs_time_from_rfc3339(buf) == milliseconds);
}
