
#ifndef RATS_PARSER_H
#define RATS_PARSER_H

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

#define RATS_REGEX_PIPE    "\\|>"
#define RATS_REGEX_STR     "'([^'\\\\]*(\\\\'[^'\\\\]*)*)'"
#define RATS_REGEX_ID      "^[a-zA-Z_][a-zA-Z0-9_]*"
#define RATS_REGEX_INT     "^[0-9]+"
#define RATS_REGEX_FLOAT   "^[0-9]+\\.[0-9]*"
#define RATS_REGEX_TIME    "^([0-9]{4})-([0-9]{2})-([0-9]{2})T([0-9]{2}):([0-9]{2}):([0-9]{2})(\\.[0-9]+)?(Z|([+-])([0-9]{2}):([0-9]{2}))"

typedef enum {
    RATS_TOKEN_TYPE_NONE,
    RATS_TOKEN_TYPE_ID,
    RATS_TOKEN_TYPE_STR,
    RATS_TOKEN_TYPE_PIPE,
    RATS_TOKEN_TYPE_INT,
    RATS_TOKEN_TYPE_FLOAT,
    RATS_TOKEN_TYPE_TIME,
    RATS_TOKEN_TYPE_EOF,
    RATS_TOKEN_TYPE_ERROR
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
    rats_int64             i64;
    rats_int64             time;
    rats_uint64            u64;
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

#endif //RATS_PARSER_H
