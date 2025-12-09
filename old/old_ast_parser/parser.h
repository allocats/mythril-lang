#pragma once
#ifndef MYTHRIL_AST_PARSER_H
#define MYTHRIL_AST_PARSER_H

#include "types.h"

#include "../mythril/types.h"

AstNode* parse_module_decl(MythrilContext* ctx, Parser* p);
AstNode* parse_import_decl(MythrilContext* ctx, Parser* p);
AstNode* parse_enum_decl(MythrilContext* ctx, Parser* p);
AstNode* parse_struct_decl(MythrilContext* ctx, Parser* p);
AstNode* parse_impl_decl(MythrilContext* ctx, Parser* p);
AstNode* parse_function_decl(MythrilContext* ctx, Parser* p);
AstNode* parse_const_decl(MythrilContext* ctx, Parser* p);
AstNode* parse_static_decl(MythrilContext* ctx, Parser* p);
AstNode* parse_var_decl(MythrilContext* ctx, Parser *p);

AstVec   parse_block(MythrilContext* ctx, Parser* p);
AstNode* parse_statement(MythrilContext* ctx, Parser* p);
AstNode* parse_loop_stmt(MythrilContext* ctx, Parser* p);
AstNode* parse_while_stmt(MythrilContext* ctx, Parser* p);
AstNode* parse_for_stmt(MythrilContext* ctx, Parser* p);

AstNode* parse_expression(MythrilContext* ctx, Parser* p);
AstNode* parse_expr_prec(MythrilContext* ctx, Parser* p, u32 prec);
AstNode* parse_primary(MythrilContext* ctx, Parser* p);
AstNode* parse_postfix(MythrilContext* ctx, Parser* p, AstNode* node);

AstNode* parse_return_stmt(MythrilContext* ctx, Parser* p);

AstType* parse_type(MythrilContext* ctx, Parser* p);

u64 parse_integer(char* ptr, usize len);

//
//  tokens and token consumption, mmmmm yummy
//

bool parser_expect(MythrilContext* ctx, Parser* p, TokenKind kind, const char* what); 
bool parser_require(MythrilContext* ctx, Parser* p, TokenKind kind, const char* what);

Token* parser_previous(Parser* p);
Token* parser_peek(Parser* p);
Token* parser_peek_ahead(Parser* p);
Token* parser_advance(Parser* p);

bool parser_check(Parser* p, TokenKind kind);
bool parser_check_ahead(Parser* p, TokenKind kind);

#endif // !MYTHRIL_AST_PARSER_H
