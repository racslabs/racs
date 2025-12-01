#include "slave.h"

#include "bytes.h"

static ssize_t send_all(int fd, const void *buf, size_t len);

void racs_slaves_init(racs_slaves *slaves, const racs_config *cfg) {
    if (cfg->slaves_count > RACS_MAX_SLAVES) {
        racs_log_error("slave: max number of slaves reached");
        exit(-1);
    }

    memset(slaves, 0, sizeof(*slaves));

    for (int i = 0; i < cfg->slaves_count; ++i) {
        slaves->slaves[i] = racs_slave_open(cfg->slaves[i].host, cfg->slaves[i].port);
        slaves->size++;
    }
}

void racs_slaves_broadcast(racs_slaves *slaves, const char *data, size_t size) {
    for (int i = 0; i < slaves->size; ++i)
        racs_enqueue(&slaves->slaves[i]->q, size, (racs_uint8 *)data);
}

racs_slave *racs_slave_open(const char *host, int port) {
    racs_slave *slave = malloc(sizeof(racs_slave));
    if (!slave) {
        racs_log_fatal("slave: failed to allocate racs_slave");
        exit(-1);
    }

    racs_slave_init_socket(slave);
    racs_slave_set_socketopts(slave);
    racs_slave_connect(slave, host, port);

    racs_queue_init(&slave->q);

    pthread_t thread;
    pthread_create(&thread, NULL, racs_slave_worker, slave);
    pthread_detach(thread);

    return slave;
}

static ssize_t send_all(int fd, const void *buf, size_t len) {
    size_t sent = 0;

    while (sent < len) {
        ssize_t rc = send(fd, (const char *)buf + sent, len - sent, 0);

        if (rc < 0) {
            racs_log_error("slave: send() failed: %s", strerror(errno));
            return -1;
        }

        if (rc == 0) {
            // peer closed
            racs_log_error("slave: send() returned 0 (peer closed)");
            return -1;
        }

        sent += rc;
    }

    return sent;
}

ssize_t racs_blocking_send(int fd, const char *data, size_t size) {
    char buf[8];
    memcpy(buf, &size, 8);

    // First send the size prefix (reliable)
    if (send_all(fd, buf, 8) < 0) {
        racs_log_error("slave: failed to send length prefix");
        return -1;
    }

    // Then send the payload
    if (size > 0) {
        if (send_all(fd, data, size) < 0) {
            racs_log_error("slave: failed to send payload");
            return -1;
        }
    }

    return size;
}

void *racs_slave_worker(void *arg) {
    racs_slave *slave = (racs_slave *)arg;

    for ( ; ; ) {
        racs_queue_entry *entry = racs_dequeue(&slave->q);

        if (entry == NULL) continue;

        ssize_t rc = racs_blocking_send(slave->fd, (const char *)entry->data, entry->size);
        racs_queue_entry_destroy(entry);

        if (rc < 0) {
            racs_log_error("slave: socket failure, worker terminating");
            break;
        }
    }

    return NULL;
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

    racs_log_info("slave: connected to %s:%u", host, port);
    freeaddrinfo(res);
}

void racs_slave_set_socketopts(racs_slave *slave) {
    int on = 1;
    int bufsize = 1024 * 1024;

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
    if (setsockopt(slave->fd, SOL_SOCKET, SO_LINGER, &ling, sizeof(ling)) < 0) {
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
