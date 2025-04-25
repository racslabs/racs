
#ifndef AUXTS_PARSER_H
#define AUXTS_PARSER_H

#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <regex.h>
#include "bytes.h"
#include "time.h"

#ifdef __cplusplus
extern "C" {
#endif

#define AUXTS_REGEX_PIPE    "\\|>"
#define AUXTS_REGEX_STR     "'([^'\\\\]*(\\\\'[^'\\\\]*)*)'"
#define AUXTS_REGEX_ID      "^[a-zA-Z_][a-zA-Z0-9_]*"
#define AUXTS_REGEX_INT     "^[0-9]+"
#define AUXTS_REGEX_FLOAT   "^[0-9]+\\.[0-9]*"
#define AUXTS_REGEX_TIME    "^([0-9]{4})-([0-9]{2})-([0-9]{2})T([0-9]{2}):([0-9]{2}):([0-9]{2})(\\.[0-9]+)?(Z|([+-])([0-9]{2}):([0-9]{2}))"

typedef enum {
    AUXTS_TOKEN_TYPE_NONE,
    AUXTS_TOKEN_TYPE_ID,
    AUXTS_TOKEN_TYPE_STR,
    AUXTS_TOKEN_TYPE_PIPE,
    AUXTS_TOKEN_TYPE_INT,
    AUXTS_TOKEN_TYPE_FLOAT,
    AUXTS_TOKEN_TYPE_TIME,
    AUXTS_TOKEN_TYPE_EOF,
    AUXTS_TOKEN_TYPE_ERROR
} rats_token_type;

typedef struct {
    size_t size;
    const char* ptr;
} rats_token_;

typedef rats_token_ rats_token_id;
typedef rats_token_ rats_token_str;

typedef struct {
    const char* msg;
} rats_token_error;

typedef union {
    double              f64;
    int64_t             i64;
    int64_t             time;
    uint64_t            u64;
    rats_token_id      id;
    rats_token_str     str;
    rats_token_error   err;
} rats_token_union;

typedef struct {
    rats_token_type  type;
    rats_token_union as;
} rats_token;

typedef struct {
    const char* ptr;
    char error[255];
    regoff_t curr;
} rats_parser;

void rats_parser_init(rats_parser* parser, const char* source);
void rats_token_print(rats_token* token);
rats_token rats_parser_next_token(rats_parser* parser);

#ifdef __cplusplus
}
#endif

#endif //AUXTS_PARSER_H
