#include "ping_test.h"

void test_ping() {
    auxts_context ctx;
    auxts_context_init(&ctx);

    auxts_command_executor exec;
    auxts_command_executor_init(&exec);

    auxts_result result = auxts_command_executor_execute(&exec, &ctx, "PING");

    msgpack_unpacked msg;
    msgpack_unpacked_init(&msg);
    msgpack_unpack_next(&msg, (char*)result.data, result.size, 0);

    msgpack_object obj = msg.data;

    msgpack_str_assert("status", &obj.via.array.ptr[0].via.str);
    msgpack_str_assert("OK", &obj.via.array.ptr[1].via.str);


    msgpack_unpacked_destroy(&msg);
    auxts_command_executor_destroy(&exec);
    auxts_context_destroy(&ctx);
}
