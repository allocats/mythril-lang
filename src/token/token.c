#include "token.h"

#include <stdio.h>

void tokens_print(Tokens* tokens) {
    for (usize i = 0; i < tokens -> count; i++) {
        Token token = tokens -> items[i];

        printf(
            "%zu: Type: %s\n\tLiteral = [%.*s]\n\n",
            i,
            TOKEN_TYPES_STRINGS[token.type],
            token.len,
            token.literal
        );
    }
}
