#include "simd_test.h"

void test_simd_swap24() {
    int32_t in[5] = {131072, 0, -131072, 0, 131072};
    int32_t out1[5];
    int32_t out2[5];

    auxts_simd_swap24(in, out1, 5);
    auxts_simd_swap24(out1, out2, 5);

    for (int i = 0; i < 5; ++i) {
        assert(((out2[i] ^ 0x800000) - 0x800000) == in[i]);
    }
}

void test_simd_swap32() {
    int32_t in[4] = {131072, 0, -131072, 0};
    int32_t out1[4];
    int32_t out2[4];

    auxts_simd_swap32(in, out1, 4);
    auxts_simd_swap32(out1, out2, 4);

    for (int i = 0; i < 4; ++i) {
        assert(out2[i] == in[i]);
    }
}