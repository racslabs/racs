#include "extract_test.h"

void test_extract() {
    rats_db* db = rats_db_instance();
    rats_db_open(db, "config.yaml");
    rats_result res = rats_db_exec(db, "EXTRACT 'test' 2025-02-09T22:51:52.213Z 2025-02-09T22:51:52.215Z");

    msgpack_unpacked msg;
    msgpack_unpacked_init(&msg);
    msgpack_unpack_next(&msg, (char*)res.data, res.size, 0);

    msgpack_object obj = msg.data;

    msgpack_str_assert("i16v", &obj.via.array.ptr[0].via.str);
    assert(obj.via.array.ptr[1].via.bin.size == 44100*4);

    msgpack_unpacked_destroy(&msg);
    rats_result_destroy(&res);
    rats_db_close(db);
}

void test_extract_no_data() {
    rats_db* db = rats_db_instance();
    rats_db_open(db, "config.yaml");
    rats_result res = rats_db_exec(db, "EXTRACT 'test' 2025-02-09T22:51:52.213Z 2025-02-09T22:51:52.212Z");

    msgpack_unpacked msg;
    msgpack_unpacked_init(&msg);
    msgpack_unpack_next(&msg, (char*)res.data, res.size, 0);

    msgpack_object obj = msg.data;

    msgpack_str_assert("i16v", &obj.via.array.ptr[0].via.str);
    assert(obj.via.array.ptr[1].via.bin.size == 0);

    msgpack_unpacked_destroy(&msg);
    rats_result_destroy(&res);
    rats_db_close(db);
}

void test_extract_error() {
    rats_db* db = rats_db_instance();
    rats_db_open(db, "config.yaml");
    rats_result res = rats_db_exec(db, "EXTRACT 'test' 2025-02-09T22:5 2025-02-09T22:51:52.212Z");

    msgpack_unpacked msg;
    msgpack_unpacked_init(&msg);
    msgpack_unpack_next(&msg, (char*)res.data, res.size, 0);

    msgpack_object obj = msg.data;

    msgpack_str_assert("error", &obj.via.array.ptr[0].via.str);

    msgpack_unpacked_destroy(&msg);
    rats_result_destroy(&res);
    rats_db_close(db);
}

void test_extract_invalid_num_args() {
    rats_db* db = rats_db_instance();
    rats_db_open(db, "config.yaml");
    rats_result res = rats_db_exec(db, "EXTRACT 'tests' 2025-02-09T22:5");

    msgpack_unpacked msg;
    msgpack_unpacked_init(&msg);
    msgpack_unpack_next(&msg, (char*)res.data, res.size, 0);

    msgpack_object obj = msg.data;

    msgpack_str_assert("error", &obj.via.array.ptr[0].via.str);

    msgpack_unpacked_destroy(&msg);
    rats_result_destroy(&res);
    rats_db_close(db);
}

void test_extract_invalid_arg_type() {
    rats_db* db = rats_db_instance();
    rats_db_open(db, "config.yaml");
    rats_result res = rats_db_exec(db, "EXTRACT 'tests' 3.2 1739141512213");

    msgpack_unpacked msg;
    msgpack_unpacked_init(&msg);
    msgpack_unpack_next(&msg, (char*)res.data, res.size, 0);

    msgpack_object obj = msg.data;

    msgpack_str_assert("error", &obj.via.array.ptr[0].via.str);

    msgpack_unpacked_destroy(&msg);
    rats_result_destroy(&res);
    rats_db_close(db);
}