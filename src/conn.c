#include "conn.h"

void racs_conn_init(racs_conn *conn, int port) {
    racs_conn_init_socket(conn);
    racs_conn_init_sockopt(conn);
    racs_conn_init_ioctl(conn);

    racs_conn_bind(conn, port);
    racs_conn_listen(conn);

    racs_conn_init_fds(conn);
}

void racs_conn_init_socket(racs_conn *conn) {
    conn->new_sd = -1;
    conn->close = 0;
    conn->listen_sd = socket(AF_INET6, SOCK_STREAM, 0);
    if (conn->listen_sd < 0) {
        perror("socket() failed");
        exit(-1);
    }
}

void racs_conn_init_sockopt(racs_conn *conn) {
    int rc = setsockopt(conn->listen_sd, SOL_SOCKET, SO_REUSEADDR, &conn->on, sizeof(conn->on));
    if (rc < 0) {
        perror("setsockopt() failed");
        close(conn->listen_sd);
        exit(-1);
    }
}

void racs_conn_init_ioctl(racs_conn *conn) {
    int rc = ioctl(conn->listen_sd, FIONBIO, &conn->on);
    if (rc < 0) {
        perror("ioctl() failed");
        close(conn->listen_sd);
        exit(-1);
    }
}

void racs_conn_init_fds(racs_conn *conn) {
    memset(conn->fds, 0, sizeof(conn->fds));
    conn->fds[0].fd = conn->listen_sd;
    conn->fds[0].events = POLLIN;
    conn->timeout = -1;
    conn->nfds = 1;
}

void racs_conn_bind(racs_conn *conn, int port) {
    memset(&conn->addr, 0, sizeof(conn->addr));
    conn->addr.sin6_family = AF_INET6;
    memcpy(&conn->addr.sin6_addr, &in6addr_any, sizeof(in6addr_any));
    conn->addr.sin6_port = htons(port);

    int rc = bind(conn->listen_sd, (struct sockaddr *) &conn->addr, sizeof(conn->addr));
    if (rc < 0) {
        perror("bind() failed");
        close(conn->listen_sd);
        exit(-1);
    }
}

void racs_conn_listen(racs_conn *conn) {
    int rc = listen(conn->listen_sd, 32);
    if (rc < 0) {
        perror("listen() failed");
        close(conn->listen_sd);
        exit(-1);
    }
}
