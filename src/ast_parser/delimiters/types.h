#pragma once 
#ifndef MYTHRIL_AST_PARSER_DELIMITERS_TYPES_H
#define MYTHRIL_AST_PARSER_DELIMITERS_TYPES_H

#include "../../tokens/types.h"
#include "../../utils/types.h"

#define DELIMITER_STACK_MAX 128

typedef struct {
    // might store index + length as u32's in order to save 8 bytes per struct
    Token* token;

    /* function name, struct name, etc. might remove this */
    const char* context;
    usize length;
} Delimiter;

typedef struct {
    i32 top;
    u8 _padding[4];
    Delimiter items[DELIMITER_STACK_MAX];
} DelimiterStack;

#endif // !MYTHRIL_AST_PARSER_DELIMITERS_TYPES_H
