
#include "mp3_test.h"


void test_mp3_encode(void) {
    racs_uint8 *src;
    size_t src_size;

    racs_uint8 *decoded;
    size_t decoded_size;

    racs_uint8 *encoded;
    size_t encoded_size;

    read_file("chopin.mp3", &src, &src_size);

    int result;
    racs_mp3_format fmt;

    result = racs_mp3_decode(&fmt, src, src_size, &decoded, &decoded_size);
    TEST_ASSERT_EQUAL_INT(RACS_MP3_OK, result);

    result = racs_mp3_encode(&fmt, decoded, decoded_size, &encoded, &encoded_size);
    TEST_ASSERT_EQUAL_INT(RACS_MP3_OK, result);

    write_file("chopin-out.mp3", encoded, encoded_size);

    free(src);
    free(decoded);
    free(encoded);
}
