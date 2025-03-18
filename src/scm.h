
#ifndef AUXTS_SCM_H
#define AUXTS_SCM_H

#include <libguile.h>
#include "db.h"

SCM auxts_scm_extract(SCM scm_stream_id, SCM scm_from, SCM scm_to);

void auxts_init_scm_bindings();

#endif //AUXTS_SCM_H
