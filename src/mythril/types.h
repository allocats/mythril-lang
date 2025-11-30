#pragma once
#ifndef MYTHRIL_TYPES_H
#define MYTHRIL_TYPES_H

#include "../arena/arena.h"
#include "../ast/types.h"
#include "../diagnostics/types.h"
#include "../tokens/types.h"

typedef struct {
    ArenaAllocator* arena;
    DiagContext* diag_ctx;
    Tokens* tokens;
    Program* program;

    char* buffer_start;
    char* buffer_end;
} MythrilContext;

#endif // !MYTHRIL_TYPES_H
