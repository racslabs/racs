#include "extract_test.h"

void msgpack_str_assert(const char* expected, const msgpack_object_str* obj_str, char* buf) {
    strncpy(buf, obj_str->ptr, obj_str->size);
    buf[obj_str->size] = '\0';
    assert(strcmp(buf, expected) == 0);
}

void test_extract() {
    uint64_t stream_id[2];
    auxts_murmurhash3_x64_128((uint8_t*)"test", 4, 0, stream_id);

    auxts_context ctx;
    auxts_context_init(&ctx);

    auxts_result result = auxts_extract(&ctx, stream_id[0], "2025-02-09T22:51:52.213Z", "2025-02-09T22:51:52.215Z");

    msgpack_unpacked msg;
    msgpack_unpacked_init(&msg);
    msgpack_unpack_next(&msg, result.data, result.size, 0);

    msgpack_object obj = msg.data;

    char buf[56];
    msgpack_str_assert("status", &obj.via.array.ptr[0].via.str, buf);
    msgpack_str_assert("OK", &obj.via.array.ptr[1].via.str, buf);

    msgpack_str_assert("samples", &obj.via.array.ptr[2].via.str, buf);
    assert(obj.via.array.ptr[3].via.u64 == 132406);

    msgpack_str_assert("channels", &obj.via.array.ptr[4].via.str, buf);
    assert(obj.via.array.ptr[5].via.u64 == 2);

    msgpack_str_assert("sample_rate", &obj.via.array.ptr[6].via.str, buf);
    assert(obj.via.array.ptr[7].via.u64 == 44100);

    msgpack_str_assert("bit_depth", &obj.via.array.ptr[8].via.str, buf);
    assert(obj.via.array.ptr[9].via.u64 == 16);

    msgpack_unpacked_destroy(&msg);
    auxts_context_destroy(&ctx);
    auxts_result_destroy(&result);
}
