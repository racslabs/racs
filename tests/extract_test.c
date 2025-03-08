#include "extract_test.h"

void test_extract() {
    auxts_context ctx;
    auxts_context_init(&ctx);

    auxts_command_executor exec;
    auxts_command_executor_init(&exec);

    auxts_result result = auxts_command_executor_execute(&exec, &ctx, "EXTRACT 'test' '2025-02-09T22:51:52.213Z' '2025-02-09T22:51:52.215Z'");

    msgpack_unpacked msg;
    msgpack_unpacked_init(&msg);
    msgpack_unpack_next(&msg, (char*)result.data, result.size, 0);

    msgpack_object obj = msg.data;

    msgpack_str_assert("status", &obj.via.array.ptr[0].via.str);
    msgpack_str_assert("OK", &obj.via.array.ptr[1].via.str);
    msgpack_str_assert("samples", &obj.via.array.ptr[2].via.str);
    msgpack_str_assert("channels", &obj.via.array.ptr[4].via.str);
    msgpack_str_assert("sample_rate", &obj.via.array.ptr[6].via.str);
    msgpack_str_assert("bit_depth", &obj.via.array.ptr[8].via.str);
    assert(obj.via.array.ptr[3].via.u64 == 132406);
    assert(obj.via.array.ptr[5].via.u64 == 2);
    assert(obj.via.array.ptr[7].via.u64 == 44100);
    assert(obj.via.array.ptr[9].via.u64 == 16);

    msgpack_unpacked_destroy(&msg);
    auxts_command_executor_destroy(&exec);
    auxts_context_destroy(&ctx);
}

void test_extract_no_data() {
    auxts_context ctx;
    auxts_context_init(&ctx);

    auxts_command_executor exec;
    auxts_command_executor_init(&exec);

    auxts_result result = auxts_command_executor_execute(&exec, &ctx, "EXTRACT 'test' '2025-02-09T22:51:52.213Z' '2025-02-09T22:51:52.212Z'");

    msgpack_unpacked msg;
    msgpack_unpacked_init(&msg);
    msgpack_unpack_next(&msg, (char*)result.data, result.size, 0);

    msgpack_object obj = msg.data;

    msgpack_str_assert("status", &obj.via.array.ptr[0].via.str);
    msgpack_str_assert("NO_DATA", &obj.via.array.ptr[1].via.str);
    msgpack_str_assert("message", &obj.via.array.ptr[2].via.str);
    msgpack_str_assert("No data found", &obj.via.array.ptr[3].via.str);

    msgpack_unpacked_destroy(&msg);
    auxts_command_executor_destroy(&exec);
    auxts_context_destroy(&ctx);
}

void test_extract_error() {
    auxts_context ctx;
    auxts_context_init(&ctx);

    auxts_command_executor exec;
    auxts_command_executor_init(&exec);

    auxts_result result = auxts_command_executor_execute(&exec, &ctx, "EXTRACT 'test' '2025-02-09T22:5' '2025-02-09T22:51:52.212Z'");

    msgpack_unpacked msg;
    msgpack_unpacked_init(&msg);
    msgpack_unpack_next(&msg, (char*)result.data, result.size, 0);

    msgpack_object obj = msg.data;

    msgpack_str_assert("status", &obj.via.array.ptr[0].via.str);
    msgpack_str_assert("ERROR", &obj.via.array.ptr[1].via.str);
    msgpack_str_assert("message", &obj.via.array.ptr[2].via.str);
    msgpack_str_assert("Invalid RFC-3339 timestamp. Expected format: yyyy-MM-ddTHH:mm:ss.SSSZ", &obj.via.array.ptr[3].via.str);

    msgpack_unpacked_destroy(&msg);
    auxts_command_executor_destroy(&exec);
    auxts_context_destroy(&ctx);
}

void test_extract_invalid_num_args() {
    auxts_context ctx;
    auxts_context_init(&ctx);

    auxts_command_executor exec;
    auxts_command_executor_init(&exec);

    auxts_result result = auxts_command_executor_execute(&exec, &ctx, "EXTRACT 'test' '2025-02-09T22:5'");

    msgpack_unpacked msg;
    msgpack_unpacked_init(&msg);
    msgpack_unpack_next(&msg, (char*)result.data, result.size, 0);

    msgpack_object obj = msg.data;

    msgpack_str_assert("status", &obj.via.array.ptr[0].via.str);
    msgpack_str_assert("ERROR", &obj.via.array.ptr[1].via.str);
    msgpack_str_assert("message", &obj.via.array.ptr[2].via.str);
    msgpack_str_assert("Expected 3 args, but got 2", &obj.via.array.ptr[3].via.str);

    msgpack_unpacked_destroy(&msg);
    auxts_command_executor_destroy(&exec);
    auxts_context_destroy(&ctx);
}