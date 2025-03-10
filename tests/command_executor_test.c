#include "command_executor_test.h"

void test_command_executor_unknown_command() {
    auxts_context ctx;
    auxts_context_init(&ctx);

    auxts_command_executor exec;
    auxts_command_executor_init(&exec);

    auxts_result result = auxts_command_executor_execute(&exec, &ctx, "BOO 0.5");

    msgpack_unpacked msg;
    msgpack_unpacked_init(&msg);
    msgpack_unpack_next(&msg, (char*)result.data, result.size, 0);

    msgpack_object obj = msg.data;

    msgpack_str_assert("status", &obj.via.array.ptr[0].via.str);
    msgpack_str_assert("ERROR", &obj.via.array.ptr[1].via.str);
    msgpack_str_assert("message", &obj.via.array.ptr[2].via.str);
    msgpack_str_assert("Unknown command: BOO", &obj.via.array.ptr[3].via.str);

    msgpack_unpacked_destroy(&msg);
    auxts_command_executor_destroy(&exec);
    auxts_context_destroy(&ctx);
}

void test_command_executor_is_not_command() {
    auxts_context ctx;
    auxts_context_init(&ctx);

    auxts_command_executor exec;
    auxts_command_executor_init(&exec);

    auxts_result result = auxts_command_executor_execute(&exec, &ctx, "PING |> 0.5");

    msgpack_unpacked msg;
    msgpack_unpacked_init(&msg);
    msgpack_unpack_next(&msg, (char*)result.data, result.size, 0);

    msgpack_object obj = msg.data;

    msgpack_str_assert("status", &obj.via.array.ptr[0].via.str);
    msgpack_str_assert("ERROR", &obj.via.array.ptr[1].via.str);
    msgpack_str_assert("message", &obj.via.array.ptr[2].via.str);
    msgpack_str_assert("Token type 'float' is not a valid command.", &obj.via.array.ptr[3].via.str);

    msgpack_unpacked_destroy(&msg);
    auxts_command_executor_destroy(&exec);
    auxts_context_destroy(&ctx);
}

void test_command_executor_parse_error1() {
    auxts_context ctx;
    auxts_context_init(&ctx);

    auxts_command_executor exec;
    auxts_command_executor_init(&exec);

    auxts_result result = auxts_command_executor_execute(&exec, &ctx, "0.5 BOO");

    msgpack_unpacked msg;
    msgpack_unpacked_init(&msg);
    msgpack_unpack_next(&msg, (char*)result.data, result.size, 0);

    msgpack_object obj = msg.data;

    msgpack_str_assert("status", &obj.via.array.ptr[0].via.str);
    msgpack_str_assert("ERROR", &obj.via.array.ptr[1].via.str);
    msgpack_str_assert("message", &obj.via.array.ptr[2].via.str);
    msgpack_str_assert("Token type 'float' is not a valid command.", &obj.via.array.ptr[3].via.str);

    msgpack_unpacked_destroy(&msg);
    auxts_command_executor_destroy(&exec);
    auxts_context_destroy(&ctx);
}
