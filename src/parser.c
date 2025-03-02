#include "parser.h"

static void parser_advance(auxts_parser* parser, regoff_t step);
static int match_token(const char* ptr, const char* pattern, regmatch_t* match);
static auxts_token parser_lex_token_str(auxts_parser* parser, regmatch_t* match);
static auxts_token parser_lex_token_id(auxts_parser* parser, regmatch_t* match);
static auxts_token parser_lex_token_bin(auxts_parser* parser, regmatch_t* match);
static auxts_token parser_lex_token_int32(auxts_parser* parser, regmatch_t* match);
static auxts_token parser_lex_token_float32(auxts_parser* parser, regmatch_t* match);
static auxts_token parser_lex_token_pipe(auxts_parser* parser, regmatch_t* match);
static auxts_token parser_lex_token_eof();
static auxts_token parser_token_error(auxts_parser* parser);

void auxts_parser_init(auxts_parser* parser, const char* source) {
    parser->ptr = source;
    parser->curr = 0;
}

auxts_token auxts_parser_next_token(auxts_parser* parser) {
    regmatch_t match;

    while (isspace(*parser->ptr)) {
        parser_advance(parser, 1);
    }

    if (*parser->ptr != '\0') {
        if (isdigit(*parser->ptr)) {
            if (match_token(parser->ptr, AUXTS_REGEX_FLOAT, &match)) {
                return parser_lex_token_float32(parser, &match);
            }
            if (match_token(parser->ptr, AUXTS_REGEX_INT, &match)) {
                return parser_lex_token_int32(parser, &match);
            }
        } else if (*parser->ptr == '.') {
            if (match_token(parser->ptr, AUXTS_REGEX_FLOAT, &match)) {
                return parser_lex_token_float32(parser, &match);
            }
        } else if (isalpha(*parser->ptr) || *parser->ptr == '_') {
            if (match_token(parser->ptr, AUXTS_REGEX_ID, &match)) {
                return parser_lex_token_id(parser, &match);
            }
        }else if (*parser->ptr == '"') {
            if (match_token(parser->ptr, AUXTS_REGEX_STR, &match)) {
                return parser_lex_token_str(parser, &match);
            }
        } else if (*parser->ptr == '|') {
            if (match_token(parser->ptr, AUXTS_REGEX_PIPE, &match)) {
                return parser_lex_token_pipe(parser, &match);
            }
        } else {
            return parser_token_error(parser);
        }

        return parser_token_error(parser);
    }

    return parser_lex_token_eof();
}

void auxts_token_print(auxts_token* token) {
    switch (token->type) {
        case AUXTS_TOKEN_TYPE_ID:
            printf("[ID   ] %s\n", token->as.id.ptr);
            break;
        case AUXTS_TOKEN_TYPE_STR:
            printf("[STR  ] %s\n", token->as.str.ptr);
            break;
        case AUXTS_TOKEN_TYPE_BIN:
            printf("[BIN  ] %s\n", token->as.bin.ptr);
            break;
        case AUXTS_TOKEN_TYPE_PIPE:
            printf("[PIPE ] |>\n");
            break;
        case AUXTS_TOKEN_TYPE_INT:
            printf("[INT  ] %d\n", token->as.i32);
            break;
        case AUXTS_TOKEN_TYPE_FLOAT:
            printf("[INT  ] %f\n", token->as.f32);
            break;
        case AUXTS_TOKEN_TYPE_EOF:
            printf("[EOF  ]\n");
            break;
        case AUXTS_TOKEN_TYPE_ERROR:
            printf("[ERR  ] %s\n", token->as.err.msg);
            break;
    }
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
