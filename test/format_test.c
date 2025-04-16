#include "format_test.h"

void test_format_wav() {
    auxts_db* db = auxts_db_instance();
    auxts_db_open(db);
    auxts_result res = auxts_db_exec(db, "EXTRACT 'test' 2025-02-09T22:51:52.213Z 2025-02-09T22:51:52.214Z "
                                         "|> FORMAT 'audio/wav' 1 44100");
    msgpack_unpacked msg;
    msgpack_unpacked_init(&msg);
    msgpack_unpack_next(&msg, (char*)res.data, res.size, 0);

    msgpack_object obj = msg.data;

    uint8_t* out = auxts_deserialize_u8v(&obj, 1);
    size_t size = auxts_deserialize_u8v_size(&obj, 1);

    FILE* f = fopen("out.wav", "wb");
    fwrite(out, 1, size, f);
    fclose(f);

    msgpack_unpacked_destroy(&msg);
    auxts_result_destroy(&res);
    auxts_db_close(db);
}
