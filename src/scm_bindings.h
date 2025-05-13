
#ifndef RACS_SCM_BINDINGS_H
#define RACS_SCM_BINDINGS_H

#include "scm.h"
#include "db.h"

SCM racs_scm_extract(SCM stream_id, SCM from, SCM to);

SCM racs_scm_streamcreate(SCM stream_id, SCM sample_rate, SCM channels);

SCM racs_scm_streaminfo(SCM stream_id, SCM attr);

void racs_scm_init_bindings();

#endif //RACS_SCM_BINDINGS_H
