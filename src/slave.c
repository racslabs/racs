#include "slave.h"

int racs_slave_open(const char *host, int port) {
    int fd = racs_slave_init_socket();

    racs_slave_set_socketopts(fd);
    racs_slave_connect(fd, host, port);

    return fd;
}

void racs_slave_connect(int fd, const char *host, int port) {
    struct addrinfo hints = {0}, *res;

    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    char port_str[6];
    snprintf(port_str, sizeof(port_str), "%u", port);

    int rc = getaddrinfo(host, port_str, &hints, &res);
    if (rc != 0) {
        racs_log_fatal("slave: getaddrinfo() failed: %s", gai_strerror(rc));
        exit(-1);
    }

    rc = connect(fd, res->ai_addr, res->ai_addrlen);
    if (rc < 0) {
        racs_log_fatal("slave: connect() failed");
        close(fd);
        exit(-1);
    }

    freeaddrinfo(res);
}

void racs_slave_set_socketopts(int fd) {
    int on = 1, off = 0;
    int bufsize = 1024 * 1024; // 1 MB

    int rc = setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, &on, sizeof(on));
    if (rc < 0) {
        racs_log_fatal("slave: setsockopt TCP_NODELAY failed");
        close(fd);
        exit(-1);
    }

    rc = setsockopt(fd, SOL_SOCKET, SO_SNDBUF, &bufsize, sizeof(bufsize));
    if (rc < 0) {
        racs_log_fatal("slave: setsockopt SO_SNDBUF failed");
        close(fd);
        exit(-1);
    }

    rc = setsockopt(fd, SOL_SOCKET, SO_RCVBUF, &bufsize, sizeof(bufsize));
    if (rc < 0) {
        racs_log_fatal("slave: setsockopt SO_RCVBUF failed");
        close(fd);
        exit(-1);
    }

    struct linger ling = {0};
    rc = setsockopt(fd, SOL_SOCKET, SO_LINGER, &ling, sizeof(ling));
    if (rc < 0) {
        racs_log_fatal("slave: setsockopt SO_LINGER failed");
        close(fd);
        exit(-1);
    }
}

int racs_slave_init_socket() {
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) {
        racs_log_fatal("slave: socket() failed");
        exit(-1);
    }

    return fd;
}

size_t racs_slave_send(int fd, const char *data, size_t size) {
    size_t bytes = 0;

    // len prefix
    ssize_t rc = send(fd, &size, sizeof(size), 0);
    if (rc < 0) {
        racs_log_error("slave: send() failed");
        return -1;
    }

    while (bytes < size) {
        rc = send(fd, data + bytes, size - bytes, 0);
        if (rc < 0) {
            racs_log_error("slave: send() failed");
            return -1;
        }

        bytes += rc;
    }

    return bytes;
}