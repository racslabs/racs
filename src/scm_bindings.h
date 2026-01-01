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
#include "daw_ops.h"

SCM racs_scm_mix(SCM in_a, SCM in_b);

SCM racs_scm_gain(SCM in, SCM gain);

SCM racs_scm_trim(SCM in, SCM left_seconds, SCM right_seconds);

SCM racs_scm_fade(SCM in, SCM fade_in_seconds, SCM fade_out_seconds);

SCM racs_scm_pan(SCM in, SCM pan);

SCM racs_scm_pad(SCM in, SCM left_seconds, SCM right_seconds);

SCM racs_scm_clip(SCM in, SCM min, SCM max);

SCM racs_scm_range(SCM stream_id, SCM from, SCM to);

SCM racs_scm_metadata(SCM stream_id, SCM attr);

SCM racs_scm_stream_list(SCM pattern);

SCM racs_scm_encode(SCM data, SCM mime_type);

void racs_scm_init_bindings();

void racs_scm_init_module();

#endif //RACS_SCM_BINDINGS_H
