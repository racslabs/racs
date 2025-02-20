#include "src/timestamp.h"
#include "src/murmurhash3.h"
#include "src/memtable.h"
#include "src/hashtable.h"
#include "src/cache.h"
#include "src/extract.h"

int main(int argc, char **argv) {
    if (strcmp(argv[1], "rfc") == 0) {
        return test_rfc3339();
    }

    if (strcmp(argv[1], "murmurhash3") == 0) {
        return test_murmurhash3();
    }

    if (strcmp(argv[1], "memtable") == 0) {
        return test_multi_memtable();
    }

    if (strcmp(argv[1], "hashtable") == 0) {
        return test_hashtable();
    }

    if (strcmp(argv[1], "cache") == 0) {
        return test_lru_cache();
    }

    if (strcmp(argv[1], "extract") == 0) {
        return test_extract();
    }

    return 0;
}
