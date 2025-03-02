
#ifndef AUXTS_PARSER_H
#define AUXTS_PARSER_H

#include <stdlib.h>
#include <stdint.h>
#include <regex.h>
#include "bytes.h"

#ifdef __cplusplus
extern "C" {
#endif

#define AUXTS_REGEX_BIN     "binary:"
#define AUXTS_REGEX_PIPE    "\\|>"
#define AUXTS_REGEX_STR     "^\"([^\"]*)\""
#define AUXTS_REGEX_ID      "^[a-zA-Z_][a-zA-Z0-9_]*"
#define AUXTS_REGEX_INT     "^[0-9]+"
#define AUXTS_REGEX_FLOAT   "^[0-9]*\\.[0-9]+"

typedef enum {
    AUXTS_TOKEN_TYPE_ID,
    AUXTS_TOKEN_TYPE_STR,
    AUXTS_TOKEN_TYPE_BIN,
    AUXTS_TOKEN_TYPE_PIPE,
    AUXTS_TOKEN_TYPE_INT,
    AUXTS_TOKEN_TYPE_FLOAT,
    AUXTS_TOKEN_TYPE_ERROR
} auxts_token_type;

typedef struct {
    size_t size;
    const char* ptr;
} auxts_token_;

typedef auxts_token_ auxts_token_id;
typedef auxts_token_ auxts_token_str;
typedef auxts_token_ auxts_token_bin;

typedef struct {
    const char* msg;
} auxts_token_error;

typedef union {
    float               f32;
    int32_t             i32;
    uint32_t            u32;
    auxts_token_id      id;
    auxts_token_str     str;
    auxts_token_bin     bin;
    auxts_token_error   err;
} auxts_token_union;

typedef struct {
    auxts_token_type  type;
    auxts_token_union as;
} auxts_token;

typedef struct {
    const char* ptr;
    char error[255];
    regoff_t curr;
} auxts_parser;


#ifdef __cplusplus
}
#endif

#endif //AUXTS_PARSER_H
