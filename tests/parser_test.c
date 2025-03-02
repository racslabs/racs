#include "parser_test.h"

void test_parser() {
    uint32_t binary_size = 4;
    uint8_t binary_data[4] = {0xa, 0xb, 0xc, 0xd};

    char buf[255];
    memcpy(buf, "append binary:", 14);
    memcpy(buf + 14, &binary_size, sizeof(uint32_t));
    memcpy(buf + 18, binary_data,  4);
    buf[22] = '\0';


    auxts_parser parser;
    auxts_parser_init(&parser, buf);

    auxts_token token = auxts_parser_next_token(&parser);

    while (token.type != AUXTS_TOKEN_TYPE_EOF) {
        if (token.type == AUXTS_TOKEN_TYPE_ERROR) {
            break;
        }
        auxts_token_print(&token);
        token = auxts_parser_next_token(&parser);
        if (token.type == AUXTS_TOKEN_TYPE_BIN) {
            printf("%zu\n", token.as.bin.size);
        }
    }

}
