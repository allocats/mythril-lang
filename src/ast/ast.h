#pragma once
#ifndef MYTHRIL_AST_H
#define MYTHRIL_AST_H

#include "types.h"

#include "../ast_parser/types.h"
#include "../files/types.h"
#include "../mythril/types.h"

#define ENUM_INIT_CAP 8

void parse(MythrilContext* ctx, char** file_paths, FileBuffer* buffers, usize file_count);

AstSlice* make_slice_from_token(ArenaAllocator* arena, Token* token);

void print_program(Program* p);

#endif // !MYTHRIL_AST_H
