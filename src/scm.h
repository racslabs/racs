
#ifndef RACS_SCM_H
#define RACS_SCM_H

#include "pack.h"
#include <libguile.h>

int racs_scm_pack_s8vector(msgpack_packer *pk, SCM v);

int racs_scm_pack_u8vector(msgpack_packer *pk, SCM v);

int racs_scm_pack_s16vector(msgpack_packer *pk, SCM v);

int racs_scm_pack_u16vector(msgpack_packer *pk, SCM v);

int racs_scm_pack_s32vector(msgpack_packer *pk, SCM v);

int racs_scm_pack_u32vector(msgpack_packer *pk, SCM v);

int racs_scm_pack_f32vector(msgpack_packer *pk, SCM v);

int racs_scm_pack_c32vector(msgpack_packer *pk, SCM v);

int racs_scm_pack(msgpack_packer *pk, msgpack_sbuffer *buf, SCM x);

int racs_scm_pack_list(msgpack_packer *pk, msgpack_sbuffer *buf, SCM x);

int racs_scm_pack_element(msgpack_packer *pk, msgpack_sbuffer *buf, SCM v);

void racs_scm_propagate_error(msgpack_object *obj, racs_uint8 *data);

SCM racs_scm_eval_wrapper(void *data);

SCM racs_scm_error_handler(void *data, SCM key, SCM args);

SCM racs_scm_eval_with_error_handling(char *expr, char **error);

void racs_unescape_single_quotes(char *s);

#endif //RACS_SCM_H
