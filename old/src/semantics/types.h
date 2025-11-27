#pragma once
#ifndef SEMANTICS_TYPES_H
#define SEMANTICS_TYPES_H

#include "../arena/arena.h"
#include "../symbols/types.h"
#include "../type_system/types.h"

typedef struct {
    ArenaAllocator* arena;
    SymbolTable* symbols;

    Type** types;
    usize type_count;
    usize type_capacity;

    AstNode* current_node;

    u32 error_count;
} SemanticCtx;

#endif // !SEMANTICS_TYPES_H
