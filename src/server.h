
#ifndef RACS_SERVER_H
#define RACS_SERVER_H


#include <sys/types.h>      // for socket types
#include <sys/socket.h>     // for socket, connect, send, recv
#include <sys/ioctl.h>      // for ioctl, FIONBIO
#include <netinet/in.h>     // for sockaddr_in, htons, etc.
#include <netinet/tcp.h>    // for TCP_FASTOPEN
#include <arpa/inet.h>      // for inet_pton, inet_ntop
#include <poll.h>

#include "db.h"
#include "scm_bindings.h"
#include "log.h"
#include "version.h"

typedef struct {
    int fd;
    racs_memstream in_stream;
    racs_memstream out_stream;
} racs_conn_stream;

typedef struct {
    int listen_sd, new_sd;
    struct sockaddr_in6 addr;
} racs_conn;

void racs_help();

void racs_args(int argc, char *argv[]);

void racs_init_socketopts(racs_conn *conn);

void racs_socket_bind(racs_conn *conn, int port);

void racs_socket_listen(racs_conn *conn);

void racs_conn_stream_init(racs_conn_stream *stream);

void racs_conn_stream_reset(racs_conn_stream *stream);

int racs_recv_length_prefix(int fd, size_t *len);

int racs_recv(int fd, int len, racs_conn_stream *stream);

void racs_send_length_prefix(racs_conn_stream *stream, size_t *len);

int racs_send(int fd, racs_conn_stream *stream);

#endif //RACS_SERVER_H
