
#include "queue_test.h"


static void destroy_cb(void *data);

static void destroy_cb(void *data) {
    free(data);
}


void test_queue(void) {
    racs_queue *q = racs_queue_create(destroy_cb);

    racs_enqueue(q, strlen("1"), strdup("1"));
    racs_enqueue(q, strlen("2"), strdup("2"));
    racs_enqueue(q, strlen("3"), strdup("3"));

    racs_queue_entry *entry1 = racs_dequeue(q);
    racs_queue_entry *entry2 = racs_dequeue(q);
    racs_queue_entry *entry3 = racs_dequeue(q);

    TEST_ASSERT_EQUAL_STRING("1", (char *)entry1->data);
    TEST_ASSERT_EQUAL_STRING("2", (char *)entry2->data);
    TEST_ASSERT_EQUAL_STRING("3", (char *)entry3->data);

    racs_queue_entry_destroy(entry1);
    racs_queue_entry_destroy(entry2);
    racs_queue_entry_destroy(entry3);

    racs_queue_destroy(q);
}
