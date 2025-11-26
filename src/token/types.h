#pragma once
#ifndef TOKEN_TYPES_H
#define TOKEN_TYPES_H

#include "../utils/types.h"

#define FOREACH_TOKEN(TOKEN)\
    TOKEN(T_DOT)            \
    TOKEN(T_COMMA)          \
    TOKEN(T_COLON)          \
    TOKEN(T_SEMICOLON)      \
                            \
    TOKEN(T_LEFTPAREN)      \
    TOKEN(T_RIGHTPAREN)     \
    TOKEN(T_LEFTSQUARE)     \
    TOKEN(T_RIGHTSQUARE)    \
    TOKEN(T_LEFTBRACE)      \
    TOKEN(T_RIGHTBRACE)     \
                            \
    TOKEN(T_PERCENT)        \
    TOKEN(T_SLASH)          \
    TOKEN(T_SLASHEQUALS)    \
                            \
    TOKEN(T_AMPERSAND)      \
                            \
    TOKEN(T_ASTERIX)        \
    TOKEN(T_ASTERIXEQUALS)  \
                            \
    TOKEN(T_PLUS)           \
    TOKEN(T_PLUSPLUS)       \
    TOKEN(T_PLUSEQUALS)     \
                            \
    TOKEN(T_MINUS)          \
    TOKEN(T_MINUSMINUS)     \
    TOKEN(T_MINUSEQUALS)    \
                            \
    TOKEN(T_EQUALS)         \
    TOKEN(T_BANG)           \
    TOKEN(T_LESSTHAN)       \
    TOKEN(T_GREATERTHAN)    \
                            \
    TOKEN(T_EQUALSEQUALS)   \
    TOKEN(T_BANGEQUALS)     \
    TOKEN(T_LESSEQUALS)     \
    TOKEN(T_GREATEREQUALS)  \
                            \
    TOKEN(T_BITSHIFT_LEFT)  \
    TOKEN(T_BITSHIFT_RIGHT) \
                            \
    TOKEN(T_BIT_AND_EQUALS) \
                            \
    TOKEN(T_BIT_OR)         \
    TOKEN(T_BIT_OR_EQUALS)  \
                            \
    TOKEN(T_BIT_XOR)        \
    TOKEN(T_BIT_XOR_EQUALS) \
                            \
    TOKEN(T_BIT_NOT)        \
    TOKEN(T_BIT_NOT_EQUALS) \
                            \
    TOKEN(T_NUMBER)         \
    TOKEN(T_IDENTIFIER)     \
    TOKEN(T_STR_LITERAL)    \
                            \
    TOKEN(T_INT8)           \
    TOKEN(T_INT16)          \
    TOKEN(T_INT32)          \
    TOKEN(T_INT64)          \
                            \
    TOKEN(T_UINT8)          \
    TOKEN(T_UINT16)         \
    TOKEN(T_UINT32)         \
    TOKEN(T_UINT64)         \
                            \
    TOKEN(T_FLOAT32)        \
    TOKEN(T_FLOAT64)        \
                            \
    TOKEN(T_CHAR)           \
                            \
    TOKEN(T_BOOL)           \
                            \
    TOKEN(T_USIZE)          \
    TOKEN(T_SSIZE)          \
                            \
    TOKEN(T_VOID)           \
    TOKEN(T_LET)            \
    TOKEN(T_CONST)          \
    TOKEN(T_STATIC)         \
                            \
    TOKEN(T_TRUE)           \
    TOKEN(T_FALSE)          \
                            \
    TOKEN(T_NULL)           \
    TOKEN(T_NULLPTR)        \
    TOKEN(T_UNINIT)         \
                            \
    TOKEN(T_IMPORT)         \
    TOKEN(T_MODULE)         \
                            \
    TOKEN(T_FUNCTION)       \
    TOKEN(T_STRUCT)         \
    TOKEN(T_ENUM)           \
    TOKEN(T_IMPL)           \
                            \
    TOKEN(T_SELF)           \
                            \
    TOKEN(T_COND_AND)       \
    TOKEN(T_COND_OR)        \
                            \
    TOKEN(T_FOR)            \
    TOKEN(T_IF)             \
    TOKEN(T_WHILE)          \
                            \
    TOKEN(T_RETURN)         \
    TOKEN(T_EOF)            \
                            \
    TOKEN(TOKEN_TYPE_COUNT)

#define GENERATE_ENUM(ENUM) ENUM,
#define GENERATE_STRING(STRING) #STRING,

typedef enum {
    FOREACH_TOKEN(GENERATE_ENUM)
} TokenType;

static const char* TOKEN_TYPES_STRINGS[] = {
    FOREACH_TOKEN(GENERATE_STRING)
};

typedef struct [[gnu::aligned(16)]] {
    char* literal;
    u32 len;
    TokenType type;
} Token;

typedef struct {
    Token* items;
    usize capacity;
    usize count;
} Tokens;

#endif // !TOKEN_TYPES_H
