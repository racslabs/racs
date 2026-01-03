// RACS - Remote Audio Caching Server
// Copyright (c) 2025 RACS Labs, LLC. All rights reserved.
//
// Licensed under the RACS Source Available License (RACS-SAL-1.0).
// Non-commercial use only. Commercial use requires a paid license.
// Contact: sales@racslabs.com
//
// SPDX-License-Identifier: RACS-SAL-1.0

#ifndef RACS_SCM_H
#define RACS_SCM_H

#ifdef __cplusplus
extern "C" {
#endif

#include "pack.h"
#include <libguile.h>

int racs_scm_pack_s8vector(msgpack_packer *pk, SCM v);

int racs_scm_pack_u8vector(msgpack_packer *pk, SCM v);

int racs_scm_pack_s16vector(msgpack_packer *pk, SCM v);

int racs_scm_pack_u16vector(msgpack_packer *pk, SCM v);

int racs_scm_pack_s32vector(msgpack_packer *pk, SCM v, bool is_final);

int racs_scm_pack_u32vector(msgpack_packer *pk, SCM v);

int racs_scm_pack_f32vector(msgpack_packer *pk, SCM v);

int racs_scm_pack_c32vector(msgpack_packer *pk, SCM v);

int racs_scm_pack(msgpack_packer *pk, msgpack_sbuffer *buf, SCM x, bool is_final);

int racs_scm_pack_list(msgpack_packer *pk, msgpack_sbuffer *buf, SCM x);

int racs_scm_pack_element(msgpack_packer *pk, msgpack_sbuffer *buf, SCM v);

void racs_scm_propagate_error(msgpack_object *obj, racs_uint8 *data);

SCM racs_scm_safe_eval(void *body_str);

SCM racs_scm_error_handler(void *data, SCM key, SCM args);

SCM racs_scm_safe_eval_with_error_handling(char *expr, char **error);

void racs_unescape_single_quotes(char *s);

#ifdef __cplusplus
}
#endif

#endif //RACS_SCM_H
