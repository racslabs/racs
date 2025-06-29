
#ifndef RACS_RESULT_H
#define RACS_RESULT_H

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include "types.h"
#include "log.h"

typedef struct {
    racs_uint8 *data;
    size_t size;
} racs_result;

void racs_result_init(racs_result *result, size_t size);

void racs_result_destroy(racs_result *result);


#endif //RACS_RESULT_H
