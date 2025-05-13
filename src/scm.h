
#ifndef RACS_SCM_H
#define RACS_SCM_H

#include "serialization.h"
#include <libguile.h>

int racs_scm_serialize_s8vector(msgpack_packer *pk, SCM v);

int racs_scm_serialize_u8vector(msgpack_packer *pk, SCM v);

int racs_scm_serialize_s16vector(msgpack_packer *pk, SCM v);

int racs_scm_serialize_u16vector(msgpack_packer *pk, SCM v);

int racs_scm_serialize_s32vector(msgpack_packer *pk, SCM v);

int racs_scm_serialize_u32vector(msgpack_packer *pk, SCM v);

int racs_scm_serialize_f32vector(msgpack_packer *pk, SCM v);

int racs_scm_serialize_c32vector(msgpack_packer *pk, SCM v);

int racs_scm_serialize(msgpack_packer *pk, msgpack_sbuffer *buf, SCM x);

int racs_scm_serialize_list(msgpack_packer *pk, msgpack_sbuffer *buf, SCM x);

int racs_scm_serialize_element(msgpack_packer *pk, msgpack_sbuffer *buf, SCM v);

void racs_scm_propagate_error(msgpack_object *obj, racs_uint8 *data);

SCM racs_scm_eval_wrapper(void *data);

SCM racs_scm_error_handler(void *data, SCM key, SCM args);

SCM racs_scm_eval_with_error_handling(char *expr, char **error);

void racs_unescape_single_quotes(char *s);

#endif //RACS_SCM_H
