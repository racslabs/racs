
#ifndef RACS_TEST_UTILS_H
#define RACS_TEST_UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include "types.h"


void read_file(const char *path, racs_uint8 **out, size_t *size);

void write_file(const char *path, const racs_uint8 *data, size_t size);


#endif //RACS_TEST_UTILS_H
