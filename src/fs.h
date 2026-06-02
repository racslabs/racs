
#ifndef RACS_FS_H
#define RACS_FS_H

#ifdef __cplusplus
extern "C" {
#endif

#include <errno.h>
#include <sys/stat.h>
#include <dirent.h>
#include <string.h>
#include <stdio.h>


typedef void (*racs_fs_walk_cb)(const char *path, void *data);


char *racs_fs_fname(const char *path);

void racs_fs_mkdir(const char *path);

void racs_fs_walk(const char *path, racs_fs_walk_cb cb, void *data);


#ifdef __cplusplus
}
#endif

#endif //RACS_FS_H
