#include "parser.h"

static void parser_advance(racs_parser *parser, regoff_t step);

static int match_token(const char *ptr, const char *pattern, regmatch_t *match);

static racs_token parser_lex_token_str(racs_parser *parser, regmatch_t *match);

static racs_token parser_lex_token_id(racs_parser *parser, regmatch_t *match);

static racs_token parser_lex_token_int64(racs_parser *parser, regmatch_t *match);

static racs_token parser_lex_token_float64(racs_parser *parser, regmatch_t *match);

static racs_token parser_lex_token_time(racs_parser *parser, regmatch_t *match);

static racs_token parser_lex_token_pipe(racs_parser *parser, regmatch_t *match);

static racs_token parser_token_error(racs_parser *parser);

static racs_token parser_lex_token_eof();

static void print_n_chars(const char *str, size_t n);

void racs_parser_init(racs_parser *parser, const char *source) {
    parser->ptr = source;
    parser->curr = 0;
}

racs_token racs_parser_next_token(racs_parser *parser) {
    regmatch_t match;

    while (isspace(*parser->ptr)) {
        parser_advance(parser, 1);
    }

    if (*parser->ptr == '\0') {
        return parser_lex_token_eof();
    }

    if (isdigit(*parser->ptr)) {
        if (match_token(parser->ptr, RACS_REGEX_TIME, &match)) {
            return parser_lex_token_time(parser, &match);
        }

        if (match_token(parser->ptr, RACS_REGEX_FLOAT, &match)) {
            return parser_lex_token_float64(parser, &match);
        }

        if (match_token(parser->ptr, RACS_REGEX_INT, &match)) {
            return parser_lex_token_int64(parser, &match);
        }
    }

    if (*parser->ptr == '.') {
        if (match_token(parser->ptr, RACS_REGEX_FLOAT, &match)) {
            return parser_lex_token_float64(parser, &match);
        }
    }

    if (isalpha(*parser->ptr) || *parser->ptr == '_') {
        if (match_token(parser->ptr, RACS_REGEX_ID, &match)) {
            return parser_lex_token_id(parser, &match);
        }
    }

    if (*parser->ptr == '\'') {
        if (match_token(parser->ptr, RACS_REGEX_STR, &match)) {
            return parser_lex_token_str(parser, &match);
        }
    }

    if (*parser->ptr == '|') {
        if (match_token(parser->ptr, RACS_REGEX_PIPE, &match)) {
            return parser_lex_token_pipe(parser, &match);
        }
    }

    return parser_token_error(parser);
}

void racs_token_print(racs_token *token) {
    switch (token->type) {
        case RACS_TOKEN_TYPE_ID:
            printf("[ID   ] ");
            print_n_chars(token->as.id.ptr, token->as.id.size);
            break;
        case RACS_TOKEN_TYPE_STR:
            printf("[STR  ] ");
            print_n_chars(token->as.str.ptr, token->as.str.size);
            break;
        case RACS_TOKEN_TYPE_PIPE:
            printf("[PIPE ] |>\n");
            break;
        case RACS_TOKEN_TYPE_INT:
            printf("[INT  ] %lld\n", token->as.i64);
            break;
        case RACS_TOKEN_TYPE_FLOAT:
            printf("[FLOAT] %f\n", token->as.f64);
            break;
        case RACS_TOKEN_TYPE_EOF:
            printf("[EOF  ]\n");
            break;
        case RACS_TOKEN_TYPE_TIME:
            printf("[TIME ] %lld\n", token->as.time);
            break;
        case RACS_TOKEN_TYPE_ERROR:
            printf("[ERR  ] %s\n", token->as.err.msg);
            break;
    }
}

void print_n_chars(const char *str, size_t n) {
    for (int i = 0; i < n && str[i] != '\0'; i++) {
        printf("%c", str[i]);
    }
    printf("\n");
}

int match_token(const char *ptr, const char *pattern, regmatch_t *match) {
    regex_t regex;

    regcomp(&regex, pattern, REG_EXTENDED);
    int ret = regexec(&regex, ptr, 1, match, 0);

    regfree(&regex);

    return !ret;
}

void parser_advance(racs_parser *parser, regoff_t step) {
    parser->ptr += step;
    parser->curr += step;
}

racs_token parser_lex_token_time(racs_parser *parser, regmatch_t *match) {
    regoff_t size = match->rm_eo - match->rm_so;

    char *time_str = malloc(size + 1);
    strlcpy(time_str, parser->ptr, size + 1);

    racs_token token;
    token.type = RACS_TOKEN_TYPE_TIME;
    token.as.time = racs_time_from_rfc3339(time_str);

    parser_advance(parser, size);
    free(time_str);

    return token;
}

racs_token parser_lex_token_str(racs_parser *parser, regmatch_t *match) {
    regoff_t size = match->rm_eo - match->rm_so - 2;

    parser_advance(parser, 1);

    racs_token token;
    token.type = RACS_TOKEN_TYPE_STR;
    token.as.str.ptr = parser->ptr;
    token.as.str.size = size;

    parser_advance(parser, size + 1);

    return token;
}

racs_token parser_lex_token_id(racs_parser *parser, regmatch_t *match) {
    regoff_t size = match->rm_eo - match->rm_so;

    racs_token token;
    token.type = RACS_TOKEN_TYPE_ID;
    token.as.id.ptr = parser->ptr;
    token.as.id.size = size;

    parser_advance(parser, size);

    return token;
}

racs_token parser_lex_token_int64(racs_parser *parser, regmatch_t *match) {
    regoff_t size = match->rm_eo - match->rm_so;

    char *int_str = malloc(size + 1);
    strlcpy(int_str, parser->ptr, size + 1);

    racs_token token;
    token.type = RACS_TOKEN_TYPE_INT;
    token.as.i64 = strtoull(int_str, NULL, 10);

    parser_advance(parser, size);
    free(int_str);

    return token;
}

racs_token parser_lex_token_float64(racs_parser *parser, regmatch_t *match) {
    regoff_t size = match->rm_eo - match->rm_so;

    char *float_str = malloc(size + 1);
    strlcpy(float_str, parser->ptr, size + 1);

    racs_token token;
    token.type = RACS_TOKEN_TYPE_FLOAT;
    token.as.f64 = atof(float_str);

    parser_advance(parser, size);
    free(float_str);

    return token;
}

racs_token parser_lex_token_pipe(racs_parser *parser, regmatch_t *match) {
    regoff_t size = match->rm_eo - match->rm_so;

    racs_token token;
    token.type = RACS_TOKEN_TYPE_PIPE;

    parser_advance(parser, size);

    return token;
}

racs_token parser_lex_token_eof() {
    racs_token token;
    token.type = RACS_TOKEN_TYPE_EOF;
    return token;
}

racs_token parser_token_error(racs_parser *parser) {
    sprintf(parser->error, "Invalid token at %lld", parser->curr);
    perror(parser->error);

    racs_token token;
    token.type = RACS_TOKEN_TYPE_ERROR;
    token.as.err.msg = parser->error;

    return token;
}
