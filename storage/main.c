#include "src/rfc.h"
#include "src/murmur3.h"

int main(int argc, char **argv) {
    if (strcmp(argv[1], "rfc") == 0) {
        return test_rfc3339();
    }

    if (strcmp(argv[1], "murmu3") == 0) {
        return test_murmur3();
    }
    return 0;
}
