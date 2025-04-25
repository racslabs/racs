#include "create_test.h"

void test_create() {
    rats_db* db = rats_db_instance();
    rats_db_open(db);
    rats_result res = rats_db_exec(db, "STREAM 'test' 44100 1");

    msgpack_unpacked msg;
    msgpack_unpacked_init(&msg);
    msgpack_unpack_next(&msg, (char*)res.data, res.size, 0);

    msgpack_object obj = msg.data;

    msgpack_str_assert("null", &obj.via.array.ptr[0].via.str);

    msgpack_unpacked_destroy(&msg);
    rats_result_destroy(&res);
    rats_db_close(db);
}
