
#ifndef AUXTS_SCM_H
#define AUXTS_SCM_H

#include "serialization.h"
#include <libguile.h>

int auxts_scm_serialize_u8vector(msgpack_packer* pk, SCM v);
int auxts_scm_serialize_s16vector(msgpack_packer* pk, SCM v);
int auxts_scm_serialize_u16vector(msgpack_packer* pk, SCM v);
int auxts_scm_serialize_s32vector(msgpack_packer* pk, SCM v);
int auxts_scm_serialize_u32vector(msgpack_packer* pk, SCM v);
int auxts_scm_serialize_f32vector(msgpack_packer* pk, SCM v);
int auxts_scm_serialize_c32vector(msgpack_packer* pk, SCM v);
void auxts_scm_propagate_error(msgpack_object* obj, uint8_t* data);
int auxts_scm_serialize(msgpack_packer* pk, msgpack_sbuffer* buf, SCM x);
int auxts_scm_serialize_list(msgpack_packer* pk, msgpack_sbuffer* buf, SCM x);
int auxts_scm_serialize_element(msgpack_packer* pk, msgpack_sbuffer* buf, SCM v);

SCM auxts_scm_eval_wrapper(void* data);
SCM auxts_scm_error_handler(void *data, SCM key, SCM args);
SCM auxts_scm_eval_with_error_handling(const char* code, char** error);

#endif //AUXTS_SCM_H
