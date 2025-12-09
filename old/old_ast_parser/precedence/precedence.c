#include "precedence.h"
#include "types.h"

u8 get_precedence(TokenKind kind) {
    return PRECEDENCE_MAP[kind];
}

b32 is_right_associative(TokenKind kind) {
    return RIGHT_ASSOC_MAP[kind];
}

b32 is_binary_operator(TokenKind kind) {
    return get_precedence(kind) > 0;
}

b32 is_assignment_operator(TokenKind kind) {
    return get_precedence(kind) == 1;
}

b32 is_prefix_operator(TokenKind kind) {
    switch (kind) {
        case TOK_MINUS:
        case TOK_PLUS:
        case TOK_BANG:
        case TOK_BIT_NOT:
        case TOK_STAR:
        case TOK_AMPERSAND:
        case TOK_PLUS_PLUS:
        case TOK_MINUS_MINUS:
            return true;

        default:
            return false;
    }
}

b32 is_postfix_operator(TokenKind kind) {
    switch (kind) {
        case TOK_PLUS_PLUS:
        case TOK_MINUS_MINUS:
        case TOK_LEFT_PAREN:
        case TOK_LEFT_SQUARE:
        case TOK_DOT:
            return true;

        default:
            return false;
    }
}
