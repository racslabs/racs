
#ifndef RACS_SERVER_H
#define RACS_SERVER_H

#ifdef __cplusplus
extern "C" {
#endif

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "version.h"

void racs_help();

void racs_args(int argc, char *argv[]);

#ifdef __cplusplus
}
#endif

#endif //RACS_SERVER_H
