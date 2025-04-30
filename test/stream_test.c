#include "stream_test.h"

void test_streamopen() {
    rats_db* db = rats_db_instance();
    rats_db_open(db);
    rats_result res = rats_db_exec(db, "STREAMOPEN 'test'");

    msgpack_unpacked msg;
    msgpack_unpacked_init(&msg);
    msgpack_unpack_next(&msg, (char*)res.data, res.size, 0);

    msgpack_object obj = msg.data;

    msgpack_str_assert("null", &obj.via.array.ptr[0].via.str);

    msgpack_unpacked_destroy(&msg);
    rats_result_destroy(&res);
    rats_db_close(db);
}

void test_atsp() {
//    int size1;
//    uint8_t* data1 = read_file("../test/data/data-1.pcm", &size1);
//    rats_uint64 hash = rats_hash("test1");
//    rats_uint32 crc = crc32c(0, data1, size1);
//
//    off_t offset = 0;
//    rats_uint8* data = malloc(sizeof(rats_frame_header) + size1);
//    offset = rats_write_bin(data, "atsp", 4, offset);
//    offset = rats_write_bin(data, "123456", 6, offset);
//    offset = rats_write_uint64(data, hash, offset);
//    offset = rats_write_uint32(data, crc, offset);
//    offset = rats_write_uint16(data, 1, offset);
//    offset = rats_write_uint32(data, 44100, offset);
//    offset = rats_write_uint16(data, 16, offset);
//    offset = rats_write_uint16(data, size1, offset);
//    rats_write_bin(data, data1, size1, offset);
//
//    rats_db* db = rats_db_instance();
//    rats_db_open(db);
//
//    rats_result res0 = rats_db_exec(db, "STREAM 'test1' 44100 1");
//    rats_result res1 = rats_db_exec(db, "STREAMOPEN 'test1'");
//    rats_result res2 = rats_db_stream(db, data);
//
//    msgpack_unpacked msg;
//    msgpack_unpacked_init(&msg);
//    msgpack_unpack_next(&msg, (char*)res2.data, res2.size, 0);
//
//    msgpack_object obj = msg.data;
//
//    msgpack_str_assert("null", &obj.via.array.ptr[0].via.str);
//    msgpack_unpacked_destroy(&msg);
//
//    rats_result_destroy(&res0);
//    rats_result_destroy(&res1);
//    rats_result_destroy(&res2);
//
//    rats_db_close(db);
//    free(data1);
//    free(data);
}
