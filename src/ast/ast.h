#pragma once
#ifndef AST_H
#define AST_H

#include "types.h"

#include "../arena/arena.h"

ProgramFunctions* ast_build(
    Tokens* tokens,
    ArenaAllocator* arena
);

void print_program(
    ProgramFunctions* funcs
);

#endif // !AST_H
