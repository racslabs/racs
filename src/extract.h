
#ifndef AUXTS_EXTRACT_H
#define AUXTS_EXTRACT_H

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
#include "flac.h"
#include "metadata.h"

typedef enum {
    AUXTS_EXTRACT_STATUS_OK,
    AUXTS_EXTRACT_STATUS_NOT_FOUND
} auxts_extract_status;

int auxts_extract_pcm(auxts_cache* cache, auxts_pcm* pcm, const char* stream_id, auxts_time from, auxts_time to);
uint8_t* auxts_extract_data_from_cache_or_sstable(auxts_cache* cache, uint64_t stream_id, auxts_time time, const char* path);
void auxts_extract_process_sstable_data(auxts_pcm* pcm, uint8_t* data, uint64_t stream_id, int64_t from, int64_t to);


#endif //AUXTS_EXTRACT_H
