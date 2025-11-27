#pragma once
#ifndef CODEGEN_H
#define CODEGEN_H

#include "types.h"

#include "../ast/types.h"
#include "../semantics/types.h"

void codegen(Program* prog, SemanticCtx* sem);

void gen_function(Codegen* ctx, AstNode* node);
void gen_statement(Codegen* ctx, AstNode* node);
void gen_expr(Codegen* ctx, AstNode* expr);
void gen_fn_call(Codegen* ctx, AstNode* expr);
void gen_syscall(Codegen* ctx, AstNode* expr);

void gen_data_section(Codegen* ctx);

u64 register_string_literal(Codegen* ctx, const char* ptr, usize len);

#endif // !CODEGEN_H
