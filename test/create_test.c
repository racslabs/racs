#include "create_test.h"

void test_create() {
    auxts_db* db = auxts_db_instance();
    auxts_db_open(db);
    auxts_result res = auxts_db_execute(db, "CREATE 'my-stream' 44100 2 16");

    msgpack_unpacked msg;
    msgpack_unpacked_init(&msg);
    msgpack_unpack_next(&msg, (char*)res.data, res.size, 0);

    msgpack_object obj = msg.data;

    msgpack_str_assert("error", &obj.via.array.ptr[0].via.str);

    msgpack_unpacked_destroy(&msg);
    auxts_result_destroy(&res);
    auxts_db_close(db);
}
