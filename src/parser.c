#include "parser.h"

static void parser_advance(auxts_parser* parser, regoff_t step);
static int match_token(const char* ptr, const char* pattern, regmatch_t* match);
static auxts_token parser_lex_token_str(auxts_parser* parser, regmatch_t* match);
static auxts_token parser_lex_token_id(auxts_parser* parser, regmatch_t* match);
static auxts_token parser_lex_token_bin(auxts_parser* parser, regmatch_t* match);
static auxts_token parser_lex_token_int32(auxts_parser* parser, regmatch_t* match);
static auxts_token parser_lex_token_float32(auxts_parser* parser, regmatch_t* match);
static auxts_token parser_lex_token_pipe(auxts_parser* parser, regmatch_t* match);
static auxts_token parser_token_error(auxts_parser* parser);

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
    regoff_t size = match->rm_eo - match->rm_so;

    auxts_token token;
    token.type = AUXTS_TOKEN_TYPE_STR;
    token.as.str.ptr = parser->ptr;
    token.as.str.size = size;

    parser_advance(parser, size);

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

auxts_token parser_lex_token_bin(auxts_parser* parser, regmatch_t* match) {
    regoff_t prefix_size = match->rm_eo - match->rm_so;

    parser_advance(parser, prefix_size);

    uint32_t binary_size;
    auxts_read_uint32(&binary_size, (uint8_t*)parser->ptr, parser->curr);

    parser_advance(parser, sizeof(uint32_t));

    auxts_token token;
    token.type = AUXTS_TOKEN_TYPE_BIN;
    token.as.bin.ptr = parser->ptr;
    token.as.bin.size = binary_size;

    parser_advance(parser, binary_size);

    return token;
}

auxts_token parser_lex_token_int32(auxts_parser* parser, regmatch_t* match) {
    regoff_t size = match->rm_eo - match->rm_so;

    char* int_str = malloc(size + 1);
    strlcpy(int_str, parser->ptr, size);

    auxts_token token;
    token.type = AUXTS_TOKEN_TYPE_INT;
    token.as.i32 = atoi(int_str);

    parser_advance(parser, size);
    free(int_str);

    return token;
}

auxts_token parser_lex_token_float32(auxts_parser* parser, regmatch_t* match) {
    regoff_t size = match->rm_eo - match->rm_so;

    char* float_str = malloc(size + 1);
    strlcpy(float_str, parser->ptr, size);

    auxts_token token;
    token.type = AUXTS_TOKEN_TYPE_FLOAT;
    token.as.f32 = (float)atof(float_str);

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

auxts_token parser_token_error(auxts_parser* parser) {
    sprintf(parser->error, "Invalid token at %lld", parser->curr);
    perror(parser->error);

    auxts_token token;
    token.type = AUXTS_TOKEN_TYPE_ERROR;
    token.as.err.msg = parser->error;

    return token;
}
