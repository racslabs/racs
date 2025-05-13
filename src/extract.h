
#ifndef RACS_EXTRACT_H
#define RACS_EXTRACT_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <msgpack.h>
#include "memtable.h"
#include "filelist.h"
#include "result.h"
#include "context.h"
#include "pcm.h"
#include "streaminfo.h"

typedef enum {
    RACS_EXTRACT_STATUS_OK,
    RACS_EXTRACT_STATUS_NOT_FOUND
} racs_extract_status;

int racs_extract_pcm(racs_context *ctx, racs_pcm *pcm, const char *stream_id, racs_time from, racs_time to);

racs_uint8 *
racs_extract_from_cache_or_sstable(racs_cache *cache, racs_uint64 stream_id, racs_time time, const char *path);

void
racs_extract_process_sstable(racs_pcm *pcm, racs_uint8 *data, racs_uint64 stream_id, racs_int64 from, racs_int64 to);


#endif //RACS_EXTRACT_H
