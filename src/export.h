// RACS - Remote Audio Caching Server
// Copyright (c) 2025 RACS Labs, LLC. All rights reserved.
//
// Licensed under the RACS Source Available License (RACS-SAL-1.0).
// Non-commercial use only. Commercial use requires a paid license.
// Contact: sales@racslabs.com
//
// SPDX-License-Identifier: RACS-SAL-1.0

#ifndef RACS_EXPORT_H
#define RACS_EXPORT_H

#ifdef __cplusplus
extern "C" {
#endif

#define RACS_FORCE_INLINE inline __attribute__((always_inline))

#define RACS_PACKED __attribute__((packed))

#define RACS_ALIGN 32

#ifdef __cplusplus
}
#endif

#endif //RACS_EXPORT_H
