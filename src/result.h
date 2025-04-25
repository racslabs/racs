
#ifndef RATS_RESULT_H
#define RATS_RESULT_H

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

typedef struct {
    uint8_t* data;
    size_t size;
} rats_result;

void rats_result_init(rats_result* result, size_t size);
void rats_result_destroy(rats_result* result);


#endif //RATS_RESULT_H
