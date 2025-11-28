#include "tokens.h"
#include "types.h"

#include <stdio.h>

void print_tokens(Tokens tokens) {
    for (usize i = 0; i < tokens.count; i++) {
        Token token = tokens.items[i];

        const char* lexeme = token.kind == TOK_EOF ? "EOF" : token.lexeme;
        const i32 length = token.kind == TOK_EOF ? 3 : token.length;

        printf("Token %zu:\n", i);
        printf("  Kind: %s\n", TOKEN_KIND_STRINGS[token.kind]);
        printf("  Lexeme: \"%.*s\"\n", (i32) length, lexeme);
        printf("  Length: %zu\n\n", token.length);
    }
}
