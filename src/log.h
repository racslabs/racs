
#ifndef RACS_LOG_H
#define RACS_LOG_H

#include <pthread.h>
#include <stdarg.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include "time.h"

typedef struct {
    int fd;
    pthread_mutex_t mutex;
} racs_log;

typedef enum {
    RACS_LOG_INFO,
    RACS_LOG_WARN,
    RACS_LOG_DEBUG,
    RACS_LOG_ERROR,
    RACS_LOG_FATAL
} racs_log_level;

extern const char *const racs_log_level_string[];

extern const char *racs_log_dir;

static racs_log *_log = NULL;

#define racs_log_info(...)  racs_log_append(racs_log_instance(), RACS_LOG_INFO, __VA_ARGS__)
#define racs_log_warn(...)  racs_log_append(racs_log_instance(), RACS_LOG_WARN, __VA_ARGS__)
#define racs_log_debug(...)  racs_log_append(racs_log_instance(), RACS_LOG_DEBUG, __VA_ARGS__)
#define racs_log_error(...)  racs_log_append(racs_log_instance(), RACS_LOG_ERROR, __VA_ARGS__)
#define racs_log_fatal(...)  racs_log_append(racs_log_instance(), RACS_LOG_FATAL, __VA_ARGS__)

racs_log *racs_log_instance();

racs_log *racs_log_create();

void racs_log_destroy(racs_log *log);

void racs_log_open(racs_log *log);

void racs_log_close(racs_log *log);

void racs_log_append(racs_log *log, racs_log_level level, const char *fmt, ...);

#endif //RACS_LOG_H
