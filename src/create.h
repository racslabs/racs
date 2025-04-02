
#ifndef AUXTS_CREATE_H
#define AUXTS_CREATE_H

#include "metadata.h"
#include "time.h"

int auxts_create(const char* name, uint32_t sample_rate, uint32_t channels, uint32_t bit_depth);

#endif //AUXTS_CREATE_H
