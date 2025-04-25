#include "conn.h"

void auxts_conn_init(auxts_conn* conn, int port) {
    auxts_conn_init_socket(conn);
    auxts_conn_init_sockopt(conn);
    auxts_conn_init_ioctl(conn);

    auxts_conn_bind(conn, port);
    auxts_conn_listen(conn);

    auxts_conn_init_fds(conn);
}

void auxts_conn_init_socket(auxts_conn* conn) {
    conn->new_sd = -1;
    conn->close = 0;
    conn->listen_sd = socket(AF_INET6, SOCK_STREAM, 0);
    if (conn->listen_sd < 0) {
        perror("socket() failed");
        exit(-1);
    }
}

void auxts_conn_init_sockopt(auxts_conn* conn) {
    int rc = setsockopt(conn->listen_sd, SOL_SOCKET, SO_REUSEADDR, &conn->on, sizeof(conn->on));
    if (rc < 0) {
        perror("setsockopt() failed");
        close(conn->listen_sd);
        exit(-1);
    }
}

void auxts_conn_init_ioctl(auxts_conn* conn) {
    int rc = ioctl(conn->listen_sd, FIONBIO, &conn->on);
    if (rc < 0) {
        perror("ioctl() failed");
        close(conn->listen_sd);
        exit(-1);
    }
}

void auxts_conn_init_fds(auxts_conn* conn) {
    memset(conn->fds, 0 , sizeof(conn->fds));
    conn->fds[0].fd = conn->listen_sd;
    conn->fds[0].events = POLLIN;
    conn->timeout = -1;
    conn->nfds = 1;
}

void auxts_conn_bind(auxts_conn* conn, int port) {
    memset(&conn->addr, 0, sizeof(conn->addr));
    conn->addr.sin6_family      = AF_INET6;
    memcpy(&conn->addr.sin6_addr, &in6addr_any, sizeof(in6addr_any));
    conn->addr.sin6_port        = htons(port);

    int rc = bind(conn->listen_sd, (struct sockaddr*)&conn->addr, sizeof(conn->addr));
    if (rc < 0) {
        perror("bind() failed");
        close(conn->listen_sd);
        exit(-1);
    }
}

void auxts_conn_listen(auxts_conn* conn) {
    int rc = listen(conn->listen_sd, 32);
    if (rc < 0) {
        perror("listen() failed");
        close(conn->listen_sd);
        exit(-1);
    }
}
