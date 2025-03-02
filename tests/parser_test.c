#include "parser_test.h"

void test_parser() {
    char* str = "extract \"2025-02-09T22:51:52.213Z\" \"2025-02-09T22:51:52.215Z\" |> "
                "biquad \"lowpass\" 0.5 |> "
                "format \"audio/wav\"";

    auxts_parser parser;
    auxts_parser_init(&parser, str);

    auxts_token token = auxts_parser_next_token(&parser);

    while (token.type != AUXTS_TOKEN_TYPE_EOF) {
        auxts_token_print(&token);
        token = auxts_parser_next_token(&parser);
    }

}
