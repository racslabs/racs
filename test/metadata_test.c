#include "metadata_test.h"

void test_metadata() {
    auxts_metadata metadata;
    metadata.channels = 2;
    metadata.bit_depth = 16;
    metadata.sample_rate = 44100;
    metadata.ref = 1739141512213;
    metadata.bytes = 44100*2;

    auxts_metadata_put(&metadata, "test");
    auxts_metadata_get(&metadata, "test");

    assert(metadata.channels == 2);
    assert(metadata.bit_depth == 16);
    assert(metadata.sample_rate == 44100);
    assert(metadata.ref == 1739141512213);
    assert(metadata.bytes == 44100*2);
}

void test_metadata_attr() {
    auxts_db* db = auxts_db_instance();
    auxts_db_open(db);
    auxts_result res = auxts_db_exec(db, "METADATA 'test' 'sample_rate'");

    msgpack_unpacked msg;
    msgpack_unpacked_init(&msg);
    msgpack_unpack_next(&msg, (char*)res.data, res.size, 0);

    msgpack_object obj = msg.data;

    msgpack_str_assert("int", &obj.via.array.ptr[0].via.str);

    msgpack_unpacked_destroy(&msg);
    auxts_result_destroy(&res);
    auxts_db_close(db);
}