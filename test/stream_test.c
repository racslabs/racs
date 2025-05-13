#include "stream_test.h"

void test_streamopen() {
    racs_db* db = racs_db_instance();
    racs_db_open(db, "config.yaml");
    racs_result res = racs_db_exec(db, "STREAMOPEN 'test'");

    msgpack_unpacked msg;
    msgpack_unpacked_init(&msg);
    msgpack_unpack_next(&msg, (char*)res.data, res.size, 0);

    msgpack_object obj = msg.data;

    msgpack_str_assert("null", &obj.via.array.ptr[0].via.str);

    msgpack_unpacked_destroy(&msg);
    racs_result_destroy(&res);
    racs_db_close(db);
}

void test_atsp() {
//    int size1;
//    uint8_t* data1 = read_file("../test/data/data-1.pcm", &size1);
//    racs_uint64 hash = racs_hash("test1");
//    racs_uint32 crc = crc32c(0, data1, size1);
//
//    off_t offset = 0;
//    racs_uint8* data = malloc(sizeof(racs_frame_header) + size1);
//    offset = racs_write_bin(data, "atsp", 4, offset);
//    offset = racs_write_bin(data, "123456", 6, offset);
//    offset = racs_write_uint64(data, hash, offset);
//    offset = racs_write_uint32(data, crc, offset);
//    offset = racs_write_uint16(data, 1, offset);
//    offset = racs_write_uint32(data, 44100, offset);
//    offset = racs_write_uint16(data, 16, offset);
//    offset = racs_write_uint16(data, size1, offset);
//    racs_write_bin(data, data1, size1, offset);
//
//    racs_db* db = racs_db_instance();
//    racs_db_open(db);
//
//    racs_result res0 = racs_db_exec(db, "STREAM 'test1' 44100 1");
//    racs_result res1 = racs_db_exec(db, "STREAMOPEN 'test1'");
//    racs_result res2 = racs_db_stream(db, data);
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
//    racs_result_destroy(&res0);
//    racs_result_destroy(&res1);
//    racs_result_destroy(&res2);
//
//    racs_db_close(db);
//    free(data1);
//    free(data);
}
