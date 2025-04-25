#include "parser_test.h"

void test_parser() {

    char* str = "EXTRACT 'test' 2025-02-09T22:51:52Z 2025-02-09T22:51:52.213Z |>"
                "IIR 'cheby1' 'lowpass' 0.5 |> "
                "FORMAT 'audio/wav'";

    rats_parser parser;
    rats_parser_init(&parser, str);

    rats_token token = rats_parser_next_token(&parser);

    while (token.type != AUXTS_TOKEN_TYPE_EOF) {
        if (token.type == AUXTS_TOKEN_TYPE_ERROR) {
            break;
        }
        rats_token_print(&token);
        token = rats_parser_next_token(&parser);
    }

}
