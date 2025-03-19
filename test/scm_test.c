#include "scm_test.h"

void test_scm_extract() {
    auxts_db* db = auxts_db_instance();
    auxts_db_open(db);

    scm_init_guile();
    auxts_init_scm_bindings();

    const char* expr = "(extract 12429135405209477533 \"2025-02-09T22:51:52.213Z\" \"2025-02-09T22:51:52.215Z\")";
    SCM result = scm_c_eval_string(expr);

    char* data = (char*)SCM_BYTEVECTOR_CONTENTS(result);
    size_t size = SCM_BYTEVECTOR_LENGTH(result);

    msgpack_unpacked msg;
    msgpack_unpacked_init(&msg);
    msgpack_unpack_next(&msg, data, size, 0);

    msgpack_object obj = msg.data;

    msgpack_str_assert("status", &obj.via.array.ptr[0].via.str);
    msgpack_str_assert("OK", &obj.via.array.ptr[1].via.str);
    msgpack_str_assert("type", &obj.via.array.ptr[2].via.str);
    msgpack_str_assert("pcm", &obj.via.array.ptr[3].via.str);
    assert(obj.via.array.ptr[4].via.str.size == 1059248);

    msgpack_unpacked_destroy(&msg);
    auxts_db_close(db);
}

void test_scm_create() {
    auxts_db* db = auxts_db_instance();
    auxts_db_open(db);

    scm_init_guile();
    auxts_init_scm_bindings();

    const char* expr = "(create \"test001\" 44100 2 16)";
    SCM result = scm_c_eval_string(expr);

    char* data = (char*)SCM_BYTEVECTOR_CONTENTS(result);
    size_t size = SCM_BYTEVECTOR_LENGTH(result);

    msgpack_unpacked msg;
    msgpack_unpacked_init(&msg);
    msgpack_unpack_next(&msg, data, size, 0);

    msgpack_object obj = msg.data;

    msgpack_str_assert("status", &obj.via.array.ptr[0].via.str);
    msgpack_str_assert("ERROR", &obj.via.array.ptr[1].via.str);

    msgpack_unpacked_destroy(&msg);
    auxts_db_close(db);
}
