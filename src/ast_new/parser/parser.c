#include "parser.h"

#include "../../utils/macros.h"

[[gnu::always_inline]]
Token* parser_peek(Parser *p) {
    MEOW_ASSERT(p -> index < p -> count, "Index out of bounds in parser_peek");
    return &p -> tokens -> items[p -> index];
}

[[gnu::always_inline]]
Token* parser_advance(Parser *p) {
    MEOW_ASSERT(p -> index < p -> count, "Index out of bounds in parser_advance");
    return &p -> tokens -> items[p -> index++];
}

[[gnu::always_inline]]
b32 parser_check(Parser *p, TokenType type) {
    Token* tok = parser_peek(p);
    return tok -> type == type;
}
