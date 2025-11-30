#include "slave.h"

void racs_slave_run(racs_slave *slave) {
    pthread_t thread;

    pthread_create(&thread, NULL, racs_slave_worker, slave);
    pthread_detach(thread);
}

void racs_slave_send(racs_slave *slave, const char *data, size_t size) {
    racs_enqueue(&slave->q, size, (racs_uint8 *)data);
}

racs_slave *racs_slave_open(const char *host, int port) {
    racs_slave *slave = malloc(sizeof(racs_slave));
    if (slave == NULL) {
        racs_log_fatal("failed to allocate racs_slave");
        exit(-1);
    }

    racs_slave_init_socket(slave);
    racs_slave_set_socketopts(slave);
    racs_slave_connect(slave, host, port);

    racs_queue_init(&slave->q);
    return slave;
}

void racs_slave_connect(racs_slave *slave, const char *host, int port) {
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

    rc = connect(slave->fd, res->ai_addr, res->ai_addrlen);
    if (rc < 0) {
        racs_log_fatal("slave: connect() failed");
        close(slave->fd);
        exit(-1);
    }

    racs_log_info("slave: connected on address=%s:%u", host, port);
    freeaddrinfo(res);
}

void racs_slave_set_socketopts(racs_slave *slave) {
    int on = 1;
    int bufsize = 1024 * 1024; // 1 MB

    int rc = setsockopt(slave->fd, IPPROTO_TCP, TCP_NODELAY, &on, sizeof(on));
    if (rc < 0) {
        racs_log_fatal("slave: setsockopt TCP_NODELAY failed");
        close(slave->fd);
        exit(-1);
    }

    rc = setsockopt(slave->fd, SOL_SOCKET, SO_SNDBUF, &bufsize, sizeof(bufsize));
    if (rc < 0) {
        racs_log_fatal("slave: setsockopt SO_SNDBUF failed");
        close(slave->fd);
        exit(-1);
    }

    rc = setsockopt(slave->fd, SOL_SOCKET, SO_RCVBUF, &bufsize, sizeof(bufsize));
    if (rc < 0) {
        racs_log_fatal("slave: setsockopt SO_RCVBUF failed");
        close(slave->fd);
        exit(-1);
    }

    struct linger ling = {0};
    rc = setsockopt(slave->fd, SOL_SOCKET, SO_LINGER, &ling, sizeof(ling));
    if (rc < 0) {
        racs_log_fatal("slave: setsockopt SO_LINGER failed");
        close(slave->fd);
        exit(-1);
    }
}

void racs_slave_init_socket(racs_slave *slave) {
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) {
        racs_log_fatal("slave: socket() failed");
        exit(-1);
    }

    slave->fd = fd;
}

size_t racs_blocking_send(int fd, const char *data, size_t size) {
    size_t bytes = 0;

    // len prefix
    ssize_t rc = send(fd, &size, sizeof(size), 0);
    if (rc < 0) {
        racs_log_error("slave: send() failed");
        return -1;
    }

    if (rc == 0) {
        racs_log_error("send() returned 0, connection closed");
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

void *racs_slave_worker(void *arg) {
    racs_slave *slave = (racs_slave *)arg;

    while (1) {
        racs_queue_entry *entry = racs_dequeue(&slave->q);
        racs_blocking_send(slave->fd, (const char *)entry->data, entry->size);
        racs_queue_entry_destroy(entry);
    }
}