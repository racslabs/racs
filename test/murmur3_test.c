#include "murmur3_test.h"

void test_murmurhash3() {
    char* data = "Hello, world!";

    uint64_t out[2];
    murmur3_x64_128((uint8_t *) data, 13, 0, out);

    assert(out[0] == 17388730015462876639ULL);
}