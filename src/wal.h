
#ifndef RACS_WAL_H
#define RACS_WAL_H

#include "types.h"
#include "bytes.h"
#include "crc32c.h"
#include "log.h"
#include "streaminfo.h"

#include <pthread.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    RACS_OP_CODE_CREATE,
    RACS_OP_CODE_APPEND,
} racs_op_code;

typedef struct {
    racs_op_code op_code;
    racs_uint32  checksum;
    size_t       size;
    racs_uint8  *op;
    racs_uint64  seq;
} racs_wal_entry;

typedef struct {
    int         fd;
    racs_uint64 seq;
    size_t      size;
    pthread_mutex_t mutex;
} racs_wal;

static racs_wal *_wal = NULL;

extern const char *racs_wal_dir;

#define racs_wal_append(op_code, size, op) racs_wal_append_(racs_wal_instance(), op_code, size, op)

racs_wal *racs_wal_create();

racs_wal *racs_wal_instance();

void racs_wal_append_(racs_wal *wal, racs_op_code op_code, size_t size, racs_uint8 *op);

void racs_wal_open(racs_wal *wal);

void racs_wal_close(racs_wal *wal);

void racs_wal_destroy(racs_wal *wal);

#ifdef __cplusplus
}
#endif

#endif //RACS_WAL_H