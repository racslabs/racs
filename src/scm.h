
#ifndef AUXTS_SCM_H
#define AUXTS_SCM_H

#include <libguile.h>
#include "db.h"

SCM auxts_scm_extract(SCM scm_stream_id, SCM scm_from, SCM scm_to);
SCM auxts_scm_create(SCM scm_stream_id, SCM scm_sample_rate, SCM scm_channels, SCM scm_bit_depth);
SCM auxts_scm_execute(const char* cmd);
void auxts_scm_init_bindings();

#endif //AUXTS_SCM_H
