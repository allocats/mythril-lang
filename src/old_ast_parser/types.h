#pragma once
#ifndef MYTHRIL_AST_PARSER_TYPES_H
#define MYTHRIL_AST_PARSER_TYPES_H

#include "../arena/arena.h"
#include "../ast/types.h"
#include "../tokens/types.h"
#include "../utils/types.h"

typedef struct {
    ArenaAllocator* arena;
    Tokens* tokens;
    Program* program;
    usize index;
    usize count;
    char* path;
} Parser;

#endif // !MYTHRIL_AST_PARSER_TYPES_H
