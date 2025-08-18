
#ifndef RACS_SERVER_H
#define RACS_SERVER_H

#include "conn.h"
#include "db.h"
#include "scm_bindings.h"
#include "log.h"
#include "version.h"

typedef struct {
    int fd;
    racs_memstream in_stream;
    racs_memstream out_stream;
} racs_conn_stream;

void racs_conn_stream_init(racs_conn_stream *stream);

void racs_conn_stream_reset(racs_conn_stream *stream);

int racs_recv_length_prefix(int fd, size_t *len);

int racs_recv(int fd, int len, racs_conn_stream *stream);

void racs_send_length_prefix(racs_conn_stream *stream, size_t *len);

int racs_send(int fd, racs_conn_stream *stream);

#endif //RACS_SERVER_H
