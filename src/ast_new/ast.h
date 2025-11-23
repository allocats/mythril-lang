#pragma once
#ifndef AST_H
#define AST_H

#include "parser/types.h"
#include "types.h"

b32 ast_is_decl(AstNode* node);
b32 ast_is_stmt(AstNode* node);
b32 ast_is_expr(AstNode* node);

void ast_error(Token* tok, const char* buf, const char* msg);

Program* ast_build(ArenaAllocator* arena, Tokens* tokens, const char* buf);

AstNode* ast_create_identifier(ArenaAllocator* arena, const char* p, const usize len);

#endif // !AST_H
