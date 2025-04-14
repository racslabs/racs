#include "simd_test.h"

void test_simd_interleave() {
    auxts_int16 a[4] = {1, 2, 3, 4};
    auxts_int16 b[4] = {5, 6, 7, 8};
    auxts_int16 c[8];

    auxts_simd_interleave_s16(a, b, c, 8);

    assert(c[0] == 1);
    assert(c[1] == 5);
    assert(c[2] == 2);
    assert(c[3] == 6);
    assert(c[4] == 3);
    assert(c[5] == 7);
    assert(c[6] == 4);
    assert(c[7] == 8);
}
