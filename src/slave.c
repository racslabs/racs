#include "slave.h"

void racs_slaves_init(racs_slaves *slaves) {
    memset(slaves, 0, sizeof(racs_slaves));
}

void racs_slaves_add(racs_slaves *slaves, const char *host, int port) {
    slaves->fds[slaves->size] = racs_slave_open(host, port);
    slaves->size++;
}

void racs_slaves_dispatch(racs_slaves *slaves, char *data, size_t length) {
    for (int i = 0; i < slaves->size; i++) {
        pthread_t thread;

        racs_slave_worker_arg *arg = malloc(sizeof(racs_slave_worker_arg));
        arg->data = data;
        arg->length = length;
        arg->fd = slaves->fds[i];

        int rc = pthread_create(&thread, NULL, (void *(*)(void *))racs_slave_worker, arg);
        if (rc != 0) {
            racs_log_error("slave: failed to create thread");
        }

        pthread_detach(thread);
    }
}

void racs_slave_worker(racs_slave_worker_arg *arg) {
    racs_slave_send(arg->fd, arg->data, arg->length);
    free(arg);
}

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

    racs_log_info("slave: connected on host=%s port=%u fd=%u", host, port, fd);
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
        racs_log_error("slave: send() failed: %s", strerror(errno));
        return -1;
    }

    while (bytes < size) {
        rc = send(fd, data + bytes, size - bytes, 0);
        if (rc < 0) {
            racs_log_error("slave: send() failed: %s", strerror(errno));
            return -1;
        }

        bytes += rc;
    }

    return bytes;
}