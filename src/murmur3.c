#include "murmur3.h"

void murmur3_x64_128(const uint8_t* key, size_t len, uint32_t seed, uint64_t* out) {
    uint64_t h1 = seed;
    uint64_t h2 = seed;

    uint64_t c1 = 0x87c37b91114253d5ull;
    uint64_t c2 = 0x4cf5ad432745937full;

    size_t n = len / 16;

    const uint64_t* blocks = (const uint64_t*)key;

    for (int i = 0; i < n; i += 2) {
        uint64_t k1 = blocks[i + 0];
        uint64_t k2 = blocks[i + 1];

        k1 *= c1;
        k1  = rotl64(k1, 31);
        k1 *= c2;

        h1 ^= k1;
        h1 = rotl64(h1, 27);
        h1 += h2;
        h1 = h1 * 5 + 0x52dce729;

        k2 *= c2;
        k2  = rotl64(k2, 33);
        k2 *= c1;

        h2 ^= k2;
        h2 = rotl64(h2, 31);
        h2 += h1;
        h2 = h2 * 5 + 0x38495ab5;
    }

    uint64_t k1 = 0;
    uint64_t k2 = 0;

    switch(len & 15) {
        case 15: k2 ^= ((uint64_t)key[n + 14]) << 48;
        case 14: k2 ^= ((uint64_t)key[n + 13]) << 40;
        case 13: k2 ^= ((uint64_t)key[n + 12]) << 32;
        case 12: k2 ^= ((uint64_t)key[n + 11]) << 24;
        case 11: k2 ^= ((uint64_t)key[n + 10]) << 16;
        case 10: k2 ^= ((uint64_t)key[n +  9]) << 8;
        case  9: k2 ^= ((uint64_t)key[n +  8]) << 0;

            k2 *= c2;
            k2  = rotl64(k2, 33);
            k2 *= c1;
            h2 ^= k2;

        case  8: k1 ^= ((uint64_t)key[n +  7]) << 56;
        case  7: k1 ^= ((uint64_t)key[n +  6]) << 48;
        case  6: k1 ^= ((uint64_t)key[n +  5]) << 40;
        case  5: k1 ^= ((uint64_t)key[n +  4]) << 32;
        case  4: k1 ^= ((uint64_t)key[n +  3]) << 24;
        case  3: k1 ^= ((uint64_t)key[n +  2]) << 16;
        case  2: k1 ^= ((uint64_t)key[n +  1]) << 8;
        case  1: k1 ^= ((uint64_t)key[n +  0]) << 0;

            k1 *= c1;
            k1  = rotl64(k1, 31);
            k1 *= c2;
            h1 ^= k1;
    }

    h1 ^= len;
    h2 ^= len;

    h1 += h2;
    h2 += h1;

    h1 = fmix64(h1);
    h2 = fmix64(h2);

    h1 += h2;
    h2 += h1;

    out[0] = h1;
    out[1] = h2;
}
