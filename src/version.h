// RACS - Remote Audio Caching Server
// Copyright (c) 2025 RACS Labs, LLC. All rights reserved.
//
// Licensed under the RACS Source Available License (RACS-SAL-1.0).
// Non-commercial use only. Commercial use requires a paid license.
// Contact: sales@racslabs.com
//
// SPDX-License-Identifier: RACS-SAL-1.0

#ifndef RACS_VERSION_H
#define RACS_VERSION_H

#ifdef __cplusplus
extern "C" {
#endif

#include "export.h"
#include <stdio.h>

#define RACS_VERSION_MAJOR  0
#define RACS_VERSION_MINOR  1
#define RACS_VERSION_PATCH  0

#define RACS_VERSION ((RACS_VERSION_MAJOR * 10000) + \
                      (RACS_VERSION_MINOR * 100) +  \
                       RACS_VERSION_PATCH)

RACS_FORCE_INLINE void racs_version(char* buf) {
    sprintf(buf, "racs %d.%d.%d  (%s)", RACS_VERSION_MAJOR, RACS_VERSION_MINOR, RACS_VERSION_PATCH, __DATE__);
}

#ifdef __cplusplus
}
#endif

#endif //RACS_VERSION_H
