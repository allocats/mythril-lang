#pragma once
#ifndef MYTHRIL_AST_H
#define MYTHRIL_AST_H

#include "types.h"

#include "../ast_parser/types.h"
#include "../mythril/types.h"

void parse(MythrilContext* ctx, char** file_paths);

AstSlice* ast_make_slice_from_token(ArenaAllocator* arena, Token* token);

void print_program(Program* p);

#endif // !MYTHRIL_AST_H
