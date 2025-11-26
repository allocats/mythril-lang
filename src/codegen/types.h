#pragma once
#ifndef CODEGEN_TYPES_H
#define CODEGEN_TYPES_H

#include "../semantics/types.h"

#include <stdio.h>

typedef struct {
    const char* ptr;
    usize len;
    u64 id;
} StringLiteral;

typedef struct {
    ArenaAllocator* arena;
    FILE* output; 

    SemanticCtx* sem_ctx;

    AstNode* current_node;

    i64 stack_offset;
    i64 label_count;

    StringLiteral* strings;
    usize string_count;
    usize string_cap;
} Codegen;

#endif // !CODEGEN_TYPES_H
