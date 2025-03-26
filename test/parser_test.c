#include "parser_test.h"

void test_parser() {

    char* str = "EXTRACT 'test' 2025-02-09T22:51:52Z 2025-02-09T22:51:52.213Z |>"
                "IIR 'cheby1' 'lowpass' 0.5 |> "
                "FORMAT 'audio/wav'";

    auxts_parser parser;
    auxts_parser_init(&parser, str);

    auxts_token token = auxts_parser_next_token(&parser);

    while (token.type != AUXTS_TOKEN_TYPE_EOF) {
        if (token.type == AUXTS_TOKEN_TYPE_ERROR) {
            break;
        }
        auxts_token_print(&token);
        token = auxts_parser_next_token(&parser);
    }

}
