
#ifndef RATS_EXTRACT_H
#define RATS_EXTRACT_H

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
    RATS_EXTRACT_STATUS_OK,
    RATS_EXTRACT_STATUS_NOT_FOUND
} rats_extract_status;

int rats_extract_pcm(rats_context* ctx, rats_pcm* pcm, const char* stream_id, rats_time from, rats_time to);
rats_uint8* rats_extract_from_cache_or_sstable(rats_cache* cache, uint64_t stream_id, rats_time time, const char* path);
void rats_extract_process_sstable(rats_pcm* pcm, rats_uint8* data, uint64_t stream_id, int64_t from, int64_t to);


#endif //RATS_EXTRACT_H
