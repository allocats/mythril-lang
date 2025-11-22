#pragma once
#ifndef AST_PARSER_TYPES_H
#define AST_PARSER_TYPES_H

#include "../../arena/arena.h"
#include "../../token/types.h"
#include "../../utils/types.h"

typedef struct {
    ArenaAllocator* arena;
    Tokens* tokens;
    usize count;
    usize index;
} Parser;

#endif // !AST_PARSER_TYPES_H
