
#ifndef AUXTS_SCM_H
#define AUXTS_SCM_H

#include <libguile.h>
#include "db.h"

SCM auxts_scm_extract(SCM stream_id, SCM from, SCM to);
SCM auxts_scm_create(SCM stream_id, SCM sample_rate, SCM channels, SCM bit_depth);
SCM auxts_scm_execute(const char* cmd);
int auxts_scm_serialize_element(msgpack_packer* pk, msgpack_sbuffer* buf, SCM v);
void auxts_scm_serialize_list(msgpack_packer* pk, msgpack_sbuffer* buf, SCM x);
void auxts_scm_serialize(msgpack_packer* pk, msgpack_sbuffer* buf, SCM x);
void auxts_scm_init_bindings();

#endif //AUXTS_SCM_H
