
#include "flac_test.h"


void test_flac_decode_encode(void) {
    racs_uint8 *src;
    size_t src_size;

    racs_uint8 *decoded;
    size_t decoded_size;

    racs_uint8 *encoded;
    size_t encoded_size;

    read_file(RACS_TEST_FLAC_PATH, &src, &src_size);

    racs_flac_format fmt;

    int status = racs_flac_decode(&fmt, src, src_size, &decoded, &decoded_size);
    TEST_ASSERT_EQUAL_INT(RACS_FLAC_OK, status);

    TEST_ASSERT_EQUAL_UINT8(2, fmt.channels);
    TEST_ASSERT_EQUAL_UINT8(24, fmt.bit_depth);
    TEST_ASSERT_EQUAL_UINT32(48000, fmt.sample_rate);

    status = racs_flac_encode(&fmt, decoded, decoded_size, &encoded, &encoded_size);
    TEST_ASSERT_EQUAL_INT(RACS_FLAC_OK, status);

    write_file("chopin-out.flac", encoded, encoded_size);

    free(src);
    free(decoded);
    free(encoded);
}
