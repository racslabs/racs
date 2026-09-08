
#include "mp3_test.h"


void test_mp3_decode_encode(void) {
    racs_uint8 *src;
    size_t src_size;

    racs_uint8 *decoded;
    size_t decoded_size;

    racs_uint8 *encoded;
    size_t encoded_size;

    read_file(RACS_TEST_MP3_PATH, &src, &src_size);

    racs_mp3_format fmt;

    int status = racs_mp3_decode(&fmt, src, src_size, &decoded, &decoded_size);
    TEST_ASSERT_EQUAL_INT(RACS_MP3_OK, status);

    TEST_ASSERT_EQUAL_UINT8(2, fmt.channels);
    TEST_ASSERT_EQUAL_UINT32(48000, fmt.sample_rate);

    status = racs_mp3_encode(&fmt, decoded, decoded_size, &encoded, &encoded_size);
    TEST_ASSERT_EQUAL_INT(RACS_MP3_OK, status);

    write_file("chopin-out.mp3", encoded, encoded_size);

    free(src);
    free(decoded);
    free(encoded);
}
