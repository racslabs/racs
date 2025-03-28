
#ifndef AUXTS_DB_H
#define AUXTS_DB_H

#include "exec.h"
#include "context.h"

typedef struct {
    auxts_context ctx;
    auxts_exec exec;
} auxts_db;

static auxts_db* _db = NULL;

auxts_db* auxts_db_instance();
void auxts_db_open(auxts_db* db);
void auxts_db_close(auxts_db* db);
auxts_result auxts_db_exec(auxts_db* db, const char* cmd);





#endif //AUXTS_DB_H
