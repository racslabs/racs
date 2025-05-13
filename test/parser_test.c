#include "parser_test.h"

void test_parser() {

    char* str = "EXTRACT 'test' 2025-02-09T22:51:52Z 2025-02-09T22:51:52.213Z |>"
                "IIR 'cheby1' 'lowpass' 0.5 |> "
                "FORMAT 'audio/wav'";

    racs_parser parser;
    racs_parser_init(&parser, str);

    racs_token token = racs_parser_next_token(&parser);

    while (token.type != RACS_TOKEN_TYPE_EOF) {
        if (token.type == RACS_TOKEN_TYPE_ERROR) {
            break;
        }
        racs_token_print(&token);
        token = racs_parser_next_token(&parser);
    }

}
