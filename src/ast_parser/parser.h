#pragma once
#ifndef MYTHRIL_AST_PARSER_H
#define MYTHRIL_AST_PARSER_H

#include "types.h"

#include "../mythril/types.h"

//
//  delimiter stack
//

void delimiters_push(Parser* p, Token* token, const char* src_ctx);
Delimiter* delimiters_pop(Parser* p);

//
//  movement and token consumption
//

Token* parser_peek(Parser* p);
Token* parser_advance(Parser* p);

b8 parser_check_current(Parser* p, TokenKind kind);

//
//  ast parsing
//

AstNode* parse_module_decl(MythrilContext* ctx, Parser* p);
AstNode* parse_import_decl(MythrilContext* ctx, Parser* p);
AstNode* parse_enum_decl(MythrilContext* ctx, Parser* p);
AstNode* parse_struct_decl(MythrilContext* ctx, Parser* p);
AstNode* parse_impl_decl(MythrilContext* ctx, Parser* p);
AstNode* parse_function_decl(MythrilContext* ctx, Parser* p);
AstNode* parse_const_decl(MythrilContext* ctx, Parser* p);
AstNode* parse_static_decl(MythrilContext* ctx, Parser* p);
AstNode* parse_var_decl(MythrilContext* ctx, Parser *p);

AstNode* parse_statement(MythrilContext* ctx, Parser* p);
AstNode* parse_loop_stmt(MythrilContext* ctx, Parser* p);
AstNode* parse_while_stmt(MythrilContext* ctx, Parser* p);
AstNode* parse_for_stmt(MythrilContext* ctx, Parser* p);
AstNode* parse_return_stmt(MythrilContext* ctx, Parser* p);

AstNode* parse_expression(MythrilContext* ctx, Parser* p);
AstNode* parse_expr_prec(MythrilContext* ctx, Parser* p, u32 prec);
AstNode* parse_primary(MythrilContext* ctx, Parser* p);
AstNode* parse_postfix(MythrilContext* ctx, Parser* p, AstNode* node);

AstType* parse_type(MythrilContext* ctx, Parser* p);

//
//  errors and recovery
//

void recover_top_level_decl(Parser* p);

#endif // !MYTHRIL_AST_PARSER_H
