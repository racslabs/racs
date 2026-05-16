#include "eval_test.h"


void test_eval_ping(void) {
    msgpack_sbuffer in_buf;
    msgpack_sbuffer_init(&in_buf);

    msgpack_packer pk;
    msgpack_packer_init(&pk, &in_buf, msgpack_sbuffer_write);

    msgpack_pack_array(&pk, 1);
    msgpack_pack_str_with_body(&pk, "PING", 4);

    racs_eval_ctx ctx;
    racs_eval_ctx_init(&ctx);
    racs_eval(&ctx, (racs_uint8 *)in_buf.data, in_buf.size);

    msgpack_sbuffer_destroy(&in_buf);

    TEST_ASSERT_FALSE(ctx.has_error);

    msgpack_unpacked unpacked;
    msgpack_unpacked_init(&unpacked);

    size_t offset = 0;
    msgpack_unpack_return result = msgpack_unpack_next(
        &unpacked,
        ctx.out_buf.data,
        ctx.out_buf.size,
        &offset
    );

    TEST_ASSERT_EQUAL_INT(MSGPACK_UNPACK_SUCCESS, result);

    msgpack_object obj = unpacked.data;

    TEST_ASSERT_EQUAL_INT(MSGPACK_OBJECT_STR, obj.type);
    TEST_ASSERT_EQUAL_UINT32(4, obj.via.str.size);
    TEST_ASSERT_EQUAL_HEX8_ARRAY("PONG", obj.via.str.ptr, 4);

    msgpack_unpacked_destroy(&unpacked);
    racs_eval_ctx_cleanup(&ctx);
}
