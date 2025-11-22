#pragma once
#ifndef AST_H
#define AST_H

#include "parser/parser.h"
#include "types.h"

b32 ast_is_decl(AstNode* node);
b32 ast_is_stmt(AstNode* node);
b32 ast_is_expr(AstNode* node);

void ast_build(ArenaAllocator* arena, Tokens* tokens);

#endif // !AST_H
