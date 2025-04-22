
#ifndef AUXTS_SCM_BINDINGS_H
#define AUXTS_SCM_BINDINGS_H

#include "scm.h"
#include "db.h"

SCM auxts_scm_extract(SCM stream_id, SCM from, SCM to);
SCM auxts_scm_stream(SCM stream_id, SCM sample_rate, SCM channels);
SCM auxts_scm_streaminfo(SCM stream_id, SCM attr);
void auxts_scm_init_bindings();

#endif //AUXTS_SCM_BINDINGS_H
