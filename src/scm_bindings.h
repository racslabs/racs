// RACS - Remote Audio Caching Server
// Copyright (c) 2025 RACS Labs, LLC. All rights reserved.
//
// Licensed under the RACS Source Available License (RACS-SAL-1.0).
// Non-commercial use only. Commercial use requires a paid license.
// Contact: sales@racslabs.com
//
// SPDX-License-Identifier: RACS-SAL-1.0

#ifndef RACS_SCM_BINDINGS_H
#define RACS_SCM_BINDINGS_H

#include "scm.h"
#include "db.h"

SCM racs_scm_range(SCM stream_id, SCM from, SCM to);

SCM racs_scm_streamcreate(SCM stream_id, SCM sample_rate, SCM channels, SCM bit_depth);

SCM racs_scm_metadata(SCM stream_id, SCM attr);

SCM racs_scm_streamopen(SCM stream_id);

SCM racs_scm_streamclose(SCM stream_id);

SCM racs_scm_streamlist(SCM pattern);

SCM racs_scm_shutdown();

SCM racs_scm_ping();

SCM racs_scm_encode(SCM data, SCM mime_type, SCM sample_rate, SCM channels, SCM bit_depth);

void racs_scm_init_bindings();

void racs_scm_init_module();

#endif //RACS_SCM_BINDINGS_H
