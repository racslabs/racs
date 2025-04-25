#include "ping_test.h"

void test_ping() {
    rats_db* db = rats_db_instance();
    rats_db_open(db);
    rats_result res = rats_db_exec(db, "PING");

    msgpack_unpacked msg;
    msgpack_unpacked_init(&msg);
    msgpack_unpack_next(&msg, (char*)res.data, res.size, 0);

    msgpack_object obj = msg.data;

    msgpack_str_assert("string", &obj.via.array.ptr[0].via.str);
    msgpack_str_assert("PONG", &obj.via.array.ptr[1].via.str);

    msgpack_unpacked_destroy(&msg);
    rats_result_destroy(&res);
    rats_db_close(db);
}
