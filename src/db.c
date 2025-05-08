#include "db.h"

rats_db *rats_db_instance() {
    if (!_db) {
        _db = malloc(sizeof(rats_db));
        if (!_db) {
            perror("FATAL: Error creating db instance. shutting down");
            exit(EXIT_FAILURE);
        }
    }
    return _db;
}

void rats_db_open(rats_db *db, const char *path) {
    rats_context_init(&db->ctx, path);
    rats_exec_init(&db->exec);
}

void rats_db_close(rats_db *db) {
    rats_exec_destroy(&db->exec);
    rats_context_destroy(&db->ctx);
}

rats_result rats_db_exec(rats_db *db, const char *cmd) {
    return rats_exec_exec(&db->exec, &db->ctx, cmd);
}

rats_result rats_db_stream(rats_db *db, rats_uint8 *data) {
    return rats_exec_stream(&db->ctx, data);
}
