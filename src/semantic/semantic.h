#pragma once
#ifndef SEMANTIC_H
#define SEMANTIC_H

#include "../arena/arena.h"
#include "../ast/types.h"
#include "../symbols/types.h"

void semantic_analyze_program(
    Program* program,
    ArenaAllocator* arena,
    SymbolTable* global_table
);

#endif // !SEMANTIC_H
