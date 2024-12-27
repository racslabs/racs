#include "src/rfc.h"
#include "src/murmur3.h"
#include "src/memtable.h"
#include "src/list.h"
#include "src/hashmap.h"

int main(int argc, char **argv) {
    if (strcmp(argv[1], "rfc") == 0) {
        return test_rfc3339();
    }

    if (strcmp(argv[1], "murmur3") == 0) {
        return test_murmur3();
    }

    if (strcmp(argv[1], "memtable") == 0) {
        return test_multi_memtable();
    }

    if (strcmp(argv[1], "list") == 0) {
        return test_list();
    }

    if (strcmp(argv[1], "hashmap") == 0) {
        return test_hashmap();
    }

    return 0;
}
