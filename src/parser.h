
#ifndef RACS_PARSER_H
#define RACS_PARSER_H

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

#define RACS_REGEX_PIPE    "\\|>"
#define RACS_REGEX_STR     "'([^'\\\\]*(\\\\'[^'\\\\]*)*)'"
#define RACS_REGEX_ID      "^[a-zA-Z_][a-zA-Z0-9_]*"
#define RACS_REGEX_INT     "^[0-9]+"
#define RACS_REGEX_FLOAT   "^[0-9]+\\.[0-9]*"
#define RACS_REGEX_TIME    "^([0-9]{4})-([0-9]{2})-([0-9]{2})T([0-9]{2}):([0-9]{2}):([0-9]{2})(\\.[0-9]+)?(Z|([+-])([0-9]{2}):([0-9]{2}))"

typedef enum {
    RACS_TOKEN_TYPE_NONE,
    RACS_TOKEN_TYPE_ID,
    RACS_TOKEN_TYPE_STR,
    RACS_TOKEN_TYPE_PIPE,
    RACS_TOKEN_TYPE_INT,
    RACS_TOKEN_TYPE_FLOAT,
    RACS_TOKEN_TYPE_TIME,
    RACS_TOKEN_TYPE_EOF,
    RACS_TOKEN_TYPE_ERROR
} racs_token_type;

typedef struct {
    size_t size;
    const char *ptr;
} racs_token_;

typedef racs_token_ racs_token_id;
typedef racs_token_ racs_token_str;

typedef struct {
    const char *msg;
} racs_token_error;

typedef union {
    double f64;
    racs_int64 i64;
    racs_int64 time;
    racs_uint64 u64;
    racs_token_id id;
    racs_token_str str;
    racs_token_error err;
} racs_token_union;

typedef struct {
    racs_token_type type;
    racs_token_union as;
} racs_token;

typedef struct {
    const char *ptr;
    char error[255];
    regoff_t curr;
} racs_parser;

void racs_parser_init(racs_parser *parser, const char *source);

void racs_token_print(racs_token *token);

racs_token racs_parser_next_token(racs_parser *parser);

#ifdef __cplusplus
}
#endif

#endif //RACS_PARSER_H
