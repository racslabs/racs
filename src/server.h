// RACS - Remote Audio Caching Server
// Copyright (c) 2025 RACS Labs, LLC. All rights reserved.
//
// Licensed under the RACS Source Available License (RACS-SAL-1.0).
// Non-commercial use only. Commercial use requires a paid license.
// Contact: sales@racslabs.com
//
// SPDX-License-Identifier: RACS-SAL-1.0

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
#include "replica.h"


typedef enum {
    RACS_FD_LISTEN,
    RACS_FD_PRIMARY,
    RACS_FD_REPLICA
} racs_fd_type;

typedef struct {
    int fd;
    racs_memstream in_stream;
    racs_memstream out_stream;
    racs_uint8 prefix_buf[8];
    size_t prefix_pos;
} racs_conn_stream;

typedef struct pollfd racs_fds[200];
typedef racs_fd_type racs_fd_types[200];

typedef struct {
    bool closed;
    bool stop;
    bool compress;
    int timeout;
    int listen_sd;
    struct sockaddr_in6 addr;
    racs_fds fds;
    racs_fd_types fd_types;
} racs_conn;

void racs_help();

void racs_args(int argc, char *argv[]);

void racs_conn_init_socket(racs_conn *conn);

void racs_conn_init_fds(racs_conn *conn);

void racs_conn_set_socketopts(racs_conn *conn);

void racs_conn_set_nonblocking(racs_conn *conn);

void racs_conn_socket_bind(racs_conn *conn, int port);

void racs_conn_socket_listen(racs_conn *conn);

void racs_conn_init(racs_conn *conn, int port);

void racs_conn_stream_init(racs_conn_stream *stream);

void racs_conn_stream_reset(racs_conn_stream *stream);

int racs_recv_length_prefix(int fd, size_t *len, racs_conn_stream *stream);

int racs_recv(int fd, int len, racs_conn_stream *stream);

int racs_send(int fd, racs_conn_stream *stream);

#endif //RACS_SERVER_H
