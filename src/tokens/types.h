#pragma once
#ifndef MYTHRIL_TOKENS_TYPES_H
#define MYTHRIL_TOKENS_TYPES_H

#include "../utils/types.h"

#define X_TOKENS(X)                 \
    X(TOK_DOT)                      \
    X(TOK_DOT_DOT)                  \
    X(TOK_ELLIPSIS)                 \
                                    \
    X(TOK_ARROW)                    \
    X(TOK_COMMA)                    \
    X(TOK_COLON)                    \
    X(TOK_COLON_COLON)              \
    X(TOK_SEMI_COLON)               \
                                    \
    X(TOK_LEFT_PAREN)               \
    X(TOK_RIGHT_PAREN)              \
    X(TOK_LEFT_SQUARE)              \
    X(TOK_RIGHT_SQUARE)             \
    X(TOK_LEFT_BRACE)               \
    X(TOK_RIGHT_BRACE)              \
                                    \
    X(TOK_STRING_DELIM)             \
                                    \
    X(TOK_AMPERSAND)                \
    X(TOK_PERCENT)                  \
    X(TOK_PERCENT_EQUALS)           \
                                    \
    X(TOK_STAR)                     \
    X(TOK_STAR_EQUALS)              \
                                    \
    X(TOK_SLASH)                    \
    X(TOK_SLASH_EQUALS)             \
                                    \
    X(TOK_PLUS)                     \
    X(TOK_PLUS_PLUS)                \
    X(TOK_PLUS_EQUALS)              \
                                    \
    X(TOK_MINUS)                    \
    X(TOK_MINUS_MINUS)              \
    X(TOK_MINUS_EQUALS)             \
                                    \
    X(TOK_EQUALS)                   \
    X(TOK_EQUALS_EQUALS)            \
                                    \
    X(TOK_BANG)                     \
    X(TOK_BANG_EQUALS)              \
                                    \
    X(TOK_LESS_THAN)                \
    X(TOK_LESS_THAN_EQUALS)         \
                                    \
    X(TOK_GREATER_THAN)             \
    X(TOK_GREATER_THAN_EQUALS)      \
                                    \
    X(TOK_BIT_AND_EQUALS)           \
                                    \
    X(TOK_BIT_SHIFT_LEFT)           \
    X(TOK_BIT_SHIFT_LEFT_EQUALS)    \
                                    \
    X(TOK_BIT_SHIFT_RIGHT)          \
    X(TOK_BIT_SHIFT_RIGHT_EQUALS)   \
                                    \
    X(TOK_BIT_OR)                   \
    X(TOK_BIT_OR_EQUALS)            \
                                    \
    X(TOK_BIT_XOR)                  \
    X(TOK_BIT_XOR_EQUALS)           \
                                    \
    X(TOK_BIT_NOT)                  \
    X(TOK_BIT_NOT_EQUALS)           \
                                    \
    X(TOK_IDENTIFIER)               \
                                    \
    X(TOK_LITERAL_NUMBER)           \
    X(TOK_LITERAL_FLOAT)            \
    X(TOK_LITERAL_STRING)           \
                                    \
    X(TOK_TRUE)                     \
    X(TOK_FALSE)                    \
                                    \
    X(TOK_NULL)                     \
    X(TOK_UNINIT)                   \
                                    \
    X(TOK_MODULE)                   \
    X(TOK_IMPORT)                   \
                                    \
    X(TOK_LET)                      \
    X(TOK_CONST)                    \
    X(TOK_STATIC)                   \
                                    \
    X(TOK_FUNCTION)                 \
    X(TOK_STRUCT)                   \
    X(TOK_ENUM)                     \
    X(TOK_IMPL)                     \
    X(TOK_SELF)                     \
                                    \
    X(TOK_COND_AND)                 \
    X(TOK_COND_OR)                  \
                                    \
    X(TOK_IF)                       \
    X(TOK_MATCH)                    \
    X(TOK_LOOP)                     \
    X(TOK_WHILE)                    \
    X(TOK_FOR)                      \
    X(TOK_IN)                       \
                                    \
    X(TOK_RETURN)                   \
    X(TOK_BREAK)                    \
    X(TOK_CONTINUE)                 \
                                    \
    X(TOK_EOF)                      \
    X(TOK_EOP)                      \
    X(TOK_ERROR)                    \
    X(TOK_KIND_COUNT)

typedef enum {
    X_TOKENS(GENERATE_ENUM)
} TokenKind;

static const char* TOKEN_KIND_STRINGS[] = {
    X_TOKENS(GENERATE_STRING)
};

typedef struct {
    char* lexeme;
    usize length;
    TokenKind kind;
} Token;

typedef struct {
    Token* items;
    usize count;
    usize capacity;
} Tokens;

#endif // !MYTHRIL_TOKENS_TYPES_H
