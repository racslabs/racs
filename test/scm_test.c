#include "scm_test.h"

void test_scm_extract() {
    scm_init_guile();
    auxts_scm_init_bindings();

    auxts_db* db = auxts_db_instance();
    auxts_db_open(db);

    auxts_result res = auxts_db_exec(db, "EVAL '(extract \"test\" \"2025-02-09T22:51:52.213Z\" \"2025-02-09T22:51:52.215Z\")'");

    msgpack_unpacked msg;
    msgpack_unpacked_init(&msg);
    msgpack_unpack_next(&msg, (char*)res.data, res.size, 0);

    msgpack_object obj = msg.data;

    msgpack_str_assert("i32v", &obj.via.array.ptr[0].via.str);
    assert(obj.via.array.ptr[1].via.bin.size == 44100*4);

    msgpack_unpacked_destroy(&msg);
    auxts_result_destroy(&res);
    auxts_db_close(db);
}

void test_scm_create() {
    scm_init_guile();
    auxts_scm_init_bindings();

    auxts_db* db = auxts_db_instance();
    auxts_db_open(db);

    auxts_result res = auxts_db_exec(db, "EVAL '(create \"test\" 44100 1)'");

    msgpack_unpacked msg;
    msgpack_unpacked_init(&msg);
    msgpack_unpack_next(&msg, (char*)res.data, res.size, 0);

    msgpack_object obj = msg.data;

    msgpack_str_assert("error", &obj.via.array.ptr[0].via.str);

    msgpack_unpacked_destroy(&msg);
    auxts_result_destroy(&res);
    auxts_db_close(db);
}

void test_scm_int() {
    scm_init_guile();
    auxts_scm_init_bindings();

    auxts_db* db = auxts_db_instance();
    auxts_db_open(db);

    auxts_result res = auxts_db_exec(db, "EVAL '(+ 1 2)'");

    msgpack_unpacked msg;
    msgpack_unpacked_init(&msg);
    msgpack_unpack_next(&msg, (char*)res.data, res.size, 0);

    msgpack_object obj = msg.data;

    msgpack_str_assert("int", &obj.via.array.ptr[0].via.str);

    msgpack_unpacked_destroy(&msg);
    auxts_result_destroy(&res);
    auxts_db_close(db);
}

void test_scm_list() {
    scm_init_guile();
    auxts_scm_init_bindings();

    auxts_db* db = auxts_db_instance();
    auxts_db_open(db);

    auxts_result res = auxts_db_exec(db,"EVAL '(extract \"test\" \"2025-02-09T22:51:52.213Z\" \"2025-02-09T22:51:52.215Z\")'");

    msgpack_unpacked msg;
    msgpack_unpacked_init(&msg);
    msgpack_unpack_next(&msg, (char*)res.data, res.size, 0);

    msgpack_object obj = msg.data;

    msgpack_str_assert("i32v", &obj.via.array.ptr[0].via.str);

    msgpack_unpacked_destroy(&msg);
    auxts_result_destroy(&res);
    auxts_db_close(db);
}

void test_scm_metadata() {
    scm_init_guile();
    auxts_scm_init_bindings();

    auxts_db* db = auxts_db_instance();
    auxts_db_open(db);

    auxts_result res = auxts_db_exec(db, "EVAL '(streaminfo \"test\" \"rate\")'");

    msgpack_unpacked msg;
    msgpack_unpacked_init(&msg);
    msgpack_unpack_next(&msg, (char*)res.data, res.size, 0);

    msgpack_object obj = msg.data;

    msgpack_str_assert("int", &obj.via.array.ptr[0].via.str);
    assert(obj.via.array.ptr[1].via.u64 == 44100);

    msgpack_unpacked_destroy(&msg);
    auxts_result_destroy(&res);
    auxts_db_close(db);
}
