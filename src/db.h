
#ifndef RACS_DB_H
#define RACS_DB_H

#include "exec.h"
#include "context.h"

typedef struct {
    racs_context ctx;
    racs_exec exec;
} racs_db;

static racs_db *_db = NULL;

racs_db *racs_db_instance();

void racs_db_open(racs_db *db, const char *path);

void racs_db_close(racs_db *db);

racs_result racs_db_exec(racs_db *db, const char *cmd);

racs_result racs_db_stream(racs_db *db, racs_uint8 *data);

#endif //RACS_DB_H
