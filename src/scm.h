
#ifndef AUXTS_SCM_H
#define AUXTS_SCM_H

#include <libguile.h>
#include "db.h"

SCM auxts_scm_extract(SCM stream_id, SCM from, SCM to);
SCM auxts_scm_create(SCM stream_id, SCM sample_rate, SCM channels, SCM bit_depth);
void auxts_scm_serialize_u8vector(msgpack_packer* pk, SCM v);
void auxts_scm_serialize_s16vector(msgpack_packer* pk, SCM v);
void auxts_scm_serialize_u16vector(msgpack_packer* pk, SCM v);
void auxts_scm_serialize_s32vector(msgpack_packer* pk, SCM v);
void auxts_scm_serialize_u32vector(msgpack_packer* pk, SCM v);
void auxts_scm_serialize_f32vector(msgpack_packer* pk, SCM v);
void auxts_scm_serialize_c32vector(msgpack_packer* pk, SCM v);
void auxts_scm_serialize(msgpack_packer* pk, msgpack_sbuffer* buf, SCM x);
void auxts_scm_serialize_pair(msgpack_packer* pk, msgpack_sbuffer* buf, SCM x);
void auxts_scm_serialize_alist(msgpack_packer* pk, msgpack_sbuffer* buf, SCM x);
void auxts_scm_serialize_list(msgpack_packer* pk, msgpack_sbuffer* buf, SCM x);
int auxts_scm_serialize_element(msgpack_packer* pk, msgpack_sbuffer* buf, SCM v);
int auxts_scm_is_dotted_pair(SCM x);

void auxts_scm_propagate_error(msgpack_object* obj, uint8_t* data);
void auxts_scm_init_bindings();

#endif //AUXTS_SCM_H
