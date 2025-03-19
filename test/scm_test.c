#include "scm_test.h"

void test_scm_extract() {
    auxts_db* db = auxts_db_instance();
    auxts_db_open(db);

    scm_init_guile();
    auxts_scm_init_bindings();

    const char* expr = "(extract \"test\" \"2025-02-09T22:51:52.213Z\" \"2025-02-09T22:51:52.215Z\")";
    SCM result = scm_c_eval_string(expr);

    char* data = (char*)SCM_BYTEVECTOR_CONTENTS(result);
    size_t size = SCM_BYTEVECTOR_LENGTH(result);

    msgpack_unpacked msg;
    msgpack_unpacked_init(&msg);
    msgpack_unpack_next(&msg, data, size, 0);

    msgpack_object obj = msg.data;

    msgpack_str_assert("bin", &obj.via.array.ptr[0].via.str);
    assert(obj.via.array.ptr[1].via.str.size == 1059248);

    msgpack_unpacked_destroy(&msg);
    auxts_db_close(db);
}

void test_scm_create() {
    auxts_db* db = auxts_db_instance();
    auxts_db_open(db);

    scm_init_guile();
    auxts_scm_init_bindings();

    const char* expr = "(create \"test001\" 44100 2 16)";
    SCM result = scm_c_eval_string(expr);

    char* data = (char*)SCM_BYTEVECTOR_CONTENTS(result);
    size_t size = SCM_BYTEVECTOR_LENGTH(result);

    msgpack_unpacked msg;
    msgpack_unpacked_init(&msg);
    msgpack_unpack_next(&msg, data, size, 0);

    msgpack_object obj = msg.data;

    msgpack_str_assert("error", &obj.via.array.ptr[0].via.str);

    msgpack_unpacked_destroy(&msg);
    auxts_db_close(db);
}
