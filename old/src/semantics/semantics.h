#pragma once
#ifndef SEMANTICS_H
#define SEMANTICS_H

#include "types.h"

SemanticCtx* create_sem_ctx(ArenaAllocator* arena);

SemanticCtx* analyze_program(Program* program);

void analyze_function(SemanticCtx* ctx, AstNode* fn); 
void analyze_statement(SemanticCtx* ctx, AstNode* stmt);

Type* analyze_expr(SemanticCtx* ctx, AstNode* expr);
Type* analyze_fn_call(SemanticCtx* ctx, AstNode* node);

#endif // !SEMANTICS_H
