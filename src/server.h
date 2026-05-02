// RACS - Remote Audio Caching Server
// Copyright (c) 2025 RACS Labs, LLC. All rights reserved.
//
// Licensed under the RACS Source Available License (RACS-SAL-1.0).
// Non-commercial use only. Commercial use requires a paid license.
// Contact: sales@racslabs.com
//

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
