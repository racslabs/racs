
#ifndef AUXTS_DB_H
#define AUXTS_DB_H

#include "exec.h"
#include "context.h"

typedef struct {
    rats_context ctx;
    rats_exec exec;
} rats_db;

static rats_db* _db = NULL;

rats_db* rats_db_instance();
void rats_db_open(rats_db* db);
void rats_db_close(rats_db* db);
rats_result rats_db_exec(rats_db* db, const char* cmd);
rats_result rats_db_stream(rats_db* db, rats_uint8* data);

#endif //AUXTS_DB_H
