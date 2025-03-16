#include "db.h"

auxts_db* auxts_db_instance() {
    if (!_db) {
        _db = malloc(sizeof(auxts_db));
        if (!_db) {
            perror("FATAL: Error creating db instance. shutting down");
            exit(EXIT_FAILURE);
        }
    }
    return _db;
}

void auxts_db_open(auxts_db* db) {
    auxts_context_init(&db->ctx);
    auxts_command_executor_init(&db->exec);
}

void auxts_db_close(auxts_db* db) {
    auxts_command_executor_destroy(&db->exec);
    auxts_context_destroy(&db->ctx);
}

auxts_result auxts_db_execute(auxts_db* db, const char* cmd) {
    return auxts_command_executor_execute(&db->exec, &db->ctx, cmd);
}
