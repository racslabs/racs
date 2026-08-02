
#include "racs_string.h"


char *racs_trim(char *str) {
    if (!str) {
        return NULL;
    }

    while (*str && isspace((racs_uint8) * str)) {
        str++;
    }

    char *end = str;
    char *last = str;

    while (*end) {
        if (!isspace((racs_uint8) * end)) {
            last = end;
        }
        end++;
    }

    last[1] = '\0';
    return str;
}
