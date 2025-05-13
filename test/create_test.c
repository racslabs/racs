#include "create_test.h"

void test_create() {
    racs_db* db = racs_db_instance();
    racs_db_open(db, "config.yaml");
    racs_result res = racs_db_exec(db, "STREAMCREATE 'test' 44100 1");

    msgpack_unpacked msg;
    msgpack_unpacked_init(&msg);
    msgpack_unpack_next(&msg, (char*)res.data, res.size, 0);

    msgpack_object obj = msg.data;

    msgpack_str_assert("null", &obj.via.array.ptr[0].via.str);

    msgpack_unpacked_destroy(&msg);
    racs_result_destroy(&res);
    racs_db_close(db);
}
