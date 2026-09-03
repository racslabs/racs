
#include "opus_test.h"


void test_opus_decode_encode(void) {
    racs_uint8 *src;
    size_t src_size;

    racs_uint8 *decoded;
    size_t decoded_size;

    racs_uint8 *encoded;
    size_t encoded_size;

    read_file("chopin.opus", &src, &src_size);

    racs_opus_format fmt = {
        .sample_rate = 48000,
        .channels = 2,
        .bit_depth = 16,
    };

    int status = racs_opus_decode(&fmt, src, src_size, &decoded, &decoded_size);
    TEST_ASSERT_EQUAL_INT(RACS_OPUS_OK, status);

    TEST_ASSERT_EQUAL_UINT8(2, fmt.channels);
    TEST_ASSERT_EQUAL_UINT8(16, fmt.bit_depth);
    TEST_ASSERT_EQUAL_UINT32(48000, fmt.sample_rate);

    status = racs_opus_encode(&fmt, decoded, decoded_size, &encoded, &encoded_size);
    TEST_ASSERT_EQUAL_INT(RACS_OPUS_OK, status);

    write_file("chopin-out.opus", encoded, encoded_size);

    free(src);
    free(decoded);
    free(encoded);
}