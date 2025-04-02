#include "timestamp_test.h"

void test_rfc3339() {
    uint64_t milliseconds = 1734272943291;

    char buf[55];
    auxts_time_format_rfc3339(milliseconds, buf);

    assert(strcmp("2024-12-15T14:29:03.291Z", buf) == 0);
    assert(auxts_time_parse_rfc3339(buf) == milliseconds);
}
