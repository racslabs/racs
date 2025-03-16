
#ifndef AUXTS_TEST_UTILS_H
#define AUXTS_TEST_UTILS_H

#include <msgpack.h>
#include "../src/export.h"

AUXTS_FORCE_INLINE void msgpack_str_assert(const char* expected, const msgpack_object_str* obj_str) {
    char buf[1024];
    strncpy(buf, obj_str->ptr, obj_str->size);
    buf[obj_str->size] = '\0';
    assert(strcmp(buf, expected) == 0);
}

#endif //AUXTS_TEST_UTILS_H
