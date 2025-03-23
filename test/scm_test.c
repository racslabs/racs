#include "scm_test.h"

void test_scm_extract() {
    auxts_db* db = auxts_db_instance();
    auxts_db_open(db);

    scm_init_guile();
    auxts_scm_init_bindings();

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
    auxts_db* db = auxts_db_instance();
    auxts_db_open(db);

    scm_init_guile();
    auxts_scm_init_bindings();

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
    msgpack_sbuffer sbuf;
    msgpack_sbuffer_init(&sbuf);

    msgpack_packer pk;
    msgpack_packer_init(&pk, &sbuf, msgpack_sbuffer_write);

    scm_init_guile();

    const char* expr = "(+ 3 (- 10 6))";

    SCM result = scm_c_eval_string(expr);
    auxts_scm_serialize(&pk, &sbuf, result);

    msgpack_unpacked msg;
    msgpack_unpacked_init(&msg);
    msgpack_unpack_next(&msg, sbuf.data, sbuf.size, 0);

    msgpack_object obj = msg.data;

    msgpack_str_assert("int", &obj.via.array.ptr[0].via.str);
    assert(obj.via.array.ptr[1].via.u64 == 7);

    msgpack_unpacked_destroy(&msg);
    msgpack_sbuffer_destroy(&sbuf);
}

void test_scm_list() {
    msgpack_sbuffer sbuf;
    msgpack_sbuffer_init(&sbuf);

    msgpack_packer pk;
    msgpack_packer_init(&pk, &sbuf, msgpack_sbuffer_write);

    scm_init_guile();

    const char* expr = "(list 2  1)";

    SCM result = scm_c_eval_string(expr);
    auxts_scm_serialize(&pk, &sbuf, result);

    msgpack_unpacked msg;
    msgpack_unpacked_init(&msg);
    msgpack_unpack_next(&msg, sbuf.data, sbuf.size, 0);

    msgpack_object obj = msg.data;

    msgpack_str_assert("list", &obj.via.array.ptr[0].via.str);
//    assert(obj.via.array.ptr[1].via.u64 == 1);

    msgpack_unpacked_destroy(&msg);
}
