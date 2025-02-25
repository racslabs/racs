
#ifndef AUXTS_RESULT_H
#define AUXTS_RESULT_H

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

typedef struct {
    uint8_t* data;
    size_t size;
} auxts_result;

auxts_result* auxts_result_create(size_t size);
void auxts_result_destroy(auxts_result* result);


#endif //AUXTS_RESULT_H
