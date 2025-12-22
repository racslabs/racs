
#ifndef RACS_WAL_REPLAY_H
#define RACS_WAL_REPLAY_H

#include "stream.h"
#include "wal.h"

typedef struct {
    racs_uint32  op_code;
    racs_uint32  checksum;
    racs_uint64  size;
    racs_uint8  *op;
    racs_uint64  lsn;
} racs_wal_entry;

typedef struct {
    int         fd;
    size_t      size;
    racs_uint8 *data;
} racs_wal_segment;

void racs_wal_replay(racs_multi_memtable *mmt, racs_offsets *offset);

racs_wal_entry *racs_wal_entry_read(racs_uint8 *buf, off_t *offset);

void racs_wal_entry_destroy(racs_wal_entry *entry);

racs_wal_segment *racs_wal_segment_read(const char *path);

void racs_wal_segment_destroy(racs_wal_segment *segment);

racs_uint64 racs_wal_checkpoint_lsn();

#endif //RACS_WAL_REPLAY_H