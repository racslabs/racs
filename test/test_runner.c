#include "unity.h"
#include "queue_test.h"
#include "dict_test.h"
#include "cache_test.h"
#include "config_test.h"
#include "mt_test.h"
#include "sst_test.h"
#include "time_test.h"
#include "path_test.h"
#include "eval_test.h"
#include "fs_test.h"
#include "flush.h"
#include "mp3_test.h"
#include "aac_test.h"


void setUp(void) {
}

void tearDown(void) {
}


static void before_all(void) {
    racs_config_load("test-conf.yaml");

    racs_flush_queue_init();
    racs_flush_thread_start(racs_flush_queue_get());

    racs_mmt_init();

    racs_offsets_init();
    racs_streams_init();
}

int main(void) {
    before_all();

    UNITY_BEGIN();
    RUN_TEST(test_queue);
    RUN_TEST(test_dict_get);
    RUN_TEST(test_dict_overwrite);
    RUN_TEST(test_dict_collision);
    RUN_TEST(test_cache_get);
    RUN_TEST(test_cache_evict);
    // RUN_TEST(test_sst_read);
    RUN_TEST(test_time_from_ts);
    RUN_TEST(test_time_to_tm);
    RUN_TEST(test_time_to_rfc3339);
    RUN_TEST(test_time_from_path);
    RUN_TEST(test_config_load);
    RUN_TEST(test_path_expand);
    RUN_TEST(test_path_from_time);
    RUN_TEST(test_path_from_range);
    RUN_TEST(test_path_resolve);
    // RUN_TEST(test_mt_parts_put);
    // RUN_TEST(test_mmt_put);
    RUN_TEST(test_eval_ping);
    RUN_TEST(test_eval_create);
    RUN_TEST(test_eval_open);
    RUN_TEST(test_eval_stream);
    RUN_TEST(test_fs_fname);
    RUN_TEST(test_mp3_decode_encode);
    RUN_TEST(test_aac_decode_encode);
    return UNITY_END();
}
