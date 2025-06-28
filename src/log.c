#include "log.h"

const char *const racs_log_level_string[] = {
        "INFO",
        "WARN",
        "DEBUG",
        "ERROR",
        "FATAL"
};

racs_log *racs_log_instance() {
    if (!_log) {
        _log = racs_log_create();
    }
    return _log;
}

racs_log *racs_log_create() {
    racs_log *log = malloc(sizeof(racs_log));
    if (!log) {
        perror("Failed to create log");
        return NULL;
    }

    pthread_mutex_init(&log->mutex, NULL);
    racs_log_open(log);

    return log;
}

void racs_log_destroy(racs_log *log) {
    pthread_mutex_destroy(&log->mutex);
    racs_log_close(log);
    free(log);
}

void racs_log_close(racs_log *log) {
    close(log->fd);
}

void racs_log_open(racs_log *log) {
    char *path = NULL;
    asprintf(&path, "%s/racs.log", racs_log_dir);

    log->fd = open(path, O_WRONLY | O_CREAT | O_APPEND, 0644);
    if (log->fd == -1) {
        perror("Failed to open log file");
        free(path);
    }
}

void racs_log_append(racs_log *log, racs_log_level level, const char *fmt, ...) {
    pthread_mutex_lock(&log->mutex);

    char buf[55];
    racs_time_to_rfc3339(racs_time_now(), buf);

    char *str1 = NULL;
    char *str2 = NULL;

    va_list args;
    va_start(args, fmt);
    vasprintf(&str1, fmt, args);
    va_end(args);

    asprintf(&str2, "[%s] %s %s\n", racs_log_level_string[level], buf, str1);
    printf("%s", str2);

    write(log->fd, str2, strlen(str2));

    free(str1);
    free(str2);

    pthread_mutex_unlock(&log->mutex);
}
