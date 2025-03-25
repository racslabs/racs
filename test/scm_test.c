#include "scm_test.h"

void test_scm_extract() {
    scm_init_guile();
    auxts_scm_init_bindings();

    auxts_db* db = auxts_db_instance();
    auxts_db_open(db);

    const char* expr = "(extract \"test\" \"2025-02-09T22:51:52.213Z\" \"2025-02-09T22:51:52.215Z\")";
    SCM result = scm_c_eval_string(expr);

    msgpack_sbuffer sbuf;
    msgpack_sbuffer_init(&sbuf);

    msgpack_packer pk;
    msgpack_packer_init(&pk, &sbuf, msgpack_sbuffer_write);
    auxts_scm_serialize(&pk, &sbuf, result);

    msgpack_unpacked msg;
    msgpack_unpacked_init(&msg);
    msgpack_unpack_next(&msg, sbuf.data, sbuf.size, 0);

    msgpack_object obj = msg.data;

    msgpack_str_assert("i32v", &obj.via.array.ptr[0].via.str);
    assert(obj.via.array.ptr[1].via.bin.size == 1059248);

    msgpack_unpacked_destroy(&msg);
    msgpack_sbuffer_destroy(&sbuf);
    auxts_db_close(db);
}

void test_scm_create() {
    scm_init_guile();
    auxts_scm_init_bindings();

    auxts_db* db = auxts_db_instance();
    auxts_db_open(db);

    const char* expr = "(create \"test002\" 44100 2 16)";
    SCM result = scm_c_eval_string(expr);

    msgpack_sbuffer sbuf;
    msgpack_sbuffer_init(&sbuf);

    msgpack_packer pk;
    msgpack_packer_init(&pk, &sbuf, msgpack_sbuffer_write);
    auxts_scm_serialize(&pk, &sbuf, result);

    msgpack_unpacked msg;
    msgpack_unpacked_init(&msg);
    msgpack_unpack_next(&msg, sbuf.data, sbuf.size, 0);

    msgpack_object obj = msg.data;

    msgpack_str_assert("none", &obj.via.array.ptr[0].via.str);

    msgpack_unpacked_destroy(&msg);
    msgpack_sbuffer_destroy(&sbuf);
    auxts_db_close(db);
}

void test_scm_int() {
    scm_init_guile();
    auxts_scm_init_bindings();

    auxts_db* db = auxts_db_instance();
    auxts_db_open(db);

    auxts_result res = auxts_db_execute(db, "EVAL '\'(1 2)'");

    msgpack_unpacked msg;
    msgpack_unpacked_init(&msg);
    msgpack_unpack_next(&msg, (char*)res.data, res.size, 0);

    msgpack_object obj = msg.data;
//
    msgpack_str_assert("list", &obj.via.array.ptr[0].via.str);

    msgpack_unpacked_destroy(&msg);
    auxts_result_destroy(&res);
    auxts_db_close(db);
}

void test_scm_list() {
    scm_init_guile();
    auxts_scm_init_bindings();

    auxts_db* db = auxts_db_instance();
    auxts_db_open(db);

    auxts_result res = auxts_db_execute(db, "EVAL '(extract \"test\" \"2025-02-09T22:51:52.213Z\" \"2025-02-09T22:51:52.215Z\")'");

    msgpack_unpacked msg;
    msgpack_unpacked_init(&msg);
    msgpack_unpack_next(&msg, (char*)res.data, res.size, 0);

    msgpack_object obj = msg.data;

    msgpack_str_assert("i32v", &obj.via.array.ptr[0].via.str);

    msgpack_unpacked_destroy(&msg);
    auxts_result_destroy(&res);
    auxts_db_close(db);
}
