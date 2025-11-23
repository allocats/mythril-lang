#pragma once
#ifndef AST_PARSER_TYPES_H
#define AST_PARSER_TYPES_H

#include "../../arena/arena.h"
#include "../../token/types.h"
#include "../../utils/types.h"

typedef struct {
    ArenaAllocator* arena;
    Token* tokens;
    usize count;
    usize index;
    const char* buffer; // Buffer for error handling
} Parser;

#endif // !AST_PARSER_TYPES_H
