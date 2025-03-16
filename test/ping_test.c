#include "ping_test.h"

void test_ping() {
    auxts_db* db = auxts_db_instance();
    auxts_db_open(db);
    auxts_result res = auxts_db_execute(db, "PING");

    msgpack_unpacked msg;
    msgpack_unpacked_init(&msg);
    msgpack_unpack_next(&msg, (char*)res.data, res.size, 0);

    msgpack_object obj = msg.data;

    msgpack_str_assert("status", &obj.via.array.ptr[0].via.str);
    msgpack_str_assert("OK", &obj.via.array.ptr[1].via.str);

    msgpack_unpacked_destroy(&msg);
    auxts_result_destroy(&res);
    auxts_db_close(db);
}
