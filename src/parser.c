#include <sys/errno.h>
#include "parser.h"

static void parser_advance(auxts_parser* parser, regoff_t step);
static int match_token(const char* ptr, const char* pattern, regmatch_t* match);
static auxts_token parser_lex_token_str(auxts_parser* parser, regmatch_t* match);
static auxts_token parser_lex_token_id(auxts_parser* parser, regmatch_t* match);
static auxts_token parser_lex_token_int64(auxts_parser* parser, regmatch_t* match);
static auxts_token parser_lex_token_float64(auxts_parser* parser, regmatch_t* match);
static auxts_token parser_lex_token_pipe(auxts_parser* parser, regmatch_t* match);
static auxts_token parser_token_error(auxts_parser* parser);
static auxts_token parser_lex_token_eof();
static void print_n_chars(const char* str, size_t n);

void auxts_parser_init(auxts_parser* parser, const char* source) {
    parser->ptr = source;
    parser->curr = 0;
}

auxts_token auxts_parser_next_token(auxts_parser* parser) {
    regmatch_t match;

    while (isspace(*parser->ptr)) {
        parser_advance(parser, 1);
    }

    if (*parser->ptr == '\0') {
        return parser_lex_token_eof();
    }

    if (isdigit(*parser->ptr)) {
        if (match_token(parser->ptr, AUXTS_REGEX_FLOAT, &match)) {
            return parser_lex_token_float64(parser, &match);
        }

        if (match_token(parser->ptr, AUXTS_REGEX_INT, &match)) {
            return parser_lex_token_int64(parser, &match);
        }
    }

    if (*parser->ptr == '.') {
        if (match_token(parser->ptr, AUXTS_REGEX_FLOAT, &match)) {
            return parser_lex_token_float64(parser, &match);
        }
    }

    if (isalpha(*parser->ptr) || *parser->ptr == '_') {
        if (match_token(parser->ptr, AUXTS_REGEX_ID, &match)) {
            return parser_lex_token_id(parser, &match);
        }
    }

    if (*parser->ptr == '\'') {
        if (match_token(parser->ptr, AUXTS_REGEX_STR, &match)) {
            return parser_lex_token_str(parser, &match);
        }
    }

    if (*parser->ptr == '|') {
        if (match_token(parser->ptr, AUXTS_REGEX_PIPE, &match)) {
            return parser_lex_token_pipe(parser, &match);
        }
    }

    return parser_token_error(parser);
}

void auxts_token_print(auxts_token* token) {
    switch (token->type) {
        case AUXTS_TOKEN_TYPE_ID:
            printf("[ID   ] ");
            print_n_chars(token->as.id.ptr, token->as.id.size);
            break;
        case AUXTS_TOKEN_TYPE_STR:
            printf("[STR  ] ");
            print_n_chars(token->as.str.ptr, token->as.str.size);
            break;
        case AUXTS_TOKEN_TYPE_PIPE:
            printf("[PIPE ] |>\n");
            break;
        case AUXTS_TOKEN_TYPE_INT:
            printf("[INT  ] %lld\n", token->as.i64);
            break;
        case AUXTS_TOKEN_TYPE_FLOAT:
            printf("[FLOAT] %f\n", token->as.f64);
            break;
        case AUXTS_TOKEN_TYPE_EOF:
            printf("[EOF  ]\n");
            break;
        case AUXTS_TOKEN_TYPE_ERROR:
            printf("[ERR  ] %s\n", token->as.err.msg);
            break;
    }
}

void print_n_chars(const char* str, size_t n) {
    for (int i = 0; i < n && str[i] != '\0'; i++) {
        printf("%c", str[i]);
    }
    printf("\n");
}

int match_token(const char* ptr, const char* pattern, regmatch_t* match) {
    regex_t regex;

    regcomp(&regex, pattern, REG_EXTENDED);
    int ret = regexec(&regex, ptr, 1, match, 0);

    regfree(&regex);

    return !ret;
}

void parser_advance(auxts_parser* parser, regoff_t step) {
    parser->ptr += step;
    parser->curr += step;
}

auxts_token parser_lex_token_str(auxts_parser* parser, regmatch_t* match) {
    regoff_t size = match->rm_eo - match->rm_so - 2;

    parser_advance(parser, 1);

    auxts_token token;
    token.type = AUXTS_TOKEN_TYPE_STR;
    token.as.str.ptr = parser->ptr;
    token.as.str.size = size;

    parser_advance(parser, size + 1);

    return token;
}

auxts_token parser_lex_token_id(auxts_parser* parser, regmatch_t* match) {
    regoff_t size = match->rm_eo - match->rm_so;

    auxts_token token;
    token.type = AUXTS_TOKEN_TYPE_ID;
    token.as.id.ptr = parser->ptr;
    token.as.id.size = size;

    parser_advance(parser, size);

    return token;
}

auxts_token parser_lex_token_int64(auxts_parser* parser, regmatch_t* match) {
    regoff_t size = match->rm_eo - match->rm_so;

    char* int_str = malloc(size + 1);
    strlcpy(int_str, parser->ptr, size + 1);

    errno = 0;
    auxts_token token;
    token.type = AUXTS_TOKEN_TYPE_INT;
    token.as.i64 = strtoull(int_str, NULL, 10);

    parser_advance(parser, size);
    free(int_str);

    return token;
}

auxts_token parser_lex_token_float64(auxts_parser* parser, regmatch_t* match) {
    regoff_t size = match->rm_eo - match->rm_so;

    char* float_str = malloc(size + 1);
    strlcpy(float_str, parser->ptr, size + 1);

    auxts_token token;
    token.type = AUXTS_TOKEN_TYPE_FLOAT;
    token.as.f64 = atof(float_str);

    parser_advance(parser, size);
    free(float_str);

    return token;
}

auxts_token parser_lex_token_pipe(auxts_parser* parser, regmatch_t* match) {
    regoff_t size = match->rm_eo - match->rm_so;

    auxts_token token;
    token.type = AUXTS_TOKEN_TYPE_PIPE;

    parser_advance(parser, size);

    return token;
}

auxts_token parser_lex_token_eof() {
    auxts_token token;
    token.type = AUXTS_TOKEN_TYPE_EOF;
    return token;
}

auxts_token parser_token_error(auxts_parser* parser) {
    sprintf(parser->error, "Invalid token at %lld", parser->curr);
    perror(parser->error);

    auxts_token token;
    token.type = AUXTS_TOKEN_TYPE_ERROR;
    token.as.err.msg = parser->error;

    return token;
}
