
#ifndef RACS_CONN_H
#define RACS_CONN_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/poll.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <errno.h>
#include <string.h>
#include <netinet/tcp.h>

typedef struct {
    int len, rc, on;
    int listen_sd, new_sd;
    int desc_ready, end_server, compress_array;
    int close;
    char buffer[80];
    struct sockaddr_in6 addr;
    int timeout;
    struct pollfd fds[200];
    int nfds, current_size, i, j;
} racs_conn;

void racs_conn_init(racs_conn *conn, int port);

void racs_conn_init_socket(racs_conn *conn);

void racs_conn_init_sockopt(racs_conn *conn);

void racs_conn_init_ioctl(racs_conn *conn);

void racs_conn_bind(racs_conn *conn, int port);

void racs_conn_listen(racs_conn *conn);

void racs_conn_init_fds(racs_conn *conn);

#endif //RACS_CONN_H
