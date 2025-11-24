#pragma once
#ifndef AST_PARSER_H
#define AST_PARSER_H

#include "../types.h"
#include "types.h"

/*
*   [WARNING] HAS ASSERT
*   Get the current token
*/
Token* parser_peek(Parser* p);

/*
*   [WARNING] HAS ASSERT
*   Get the next token without incrementing the current index
*/
Token* parser_peek_ahead(Parser* p);

/*
*   [WARNING] HAS ASSERT
*   Get the current token and advance index
*/
Token* parser_advance(Parser* p);

/*
*   [WARNING] HAS ASSERT
*   Get the current token, compare its type to a desired type
*   and advance index, error is made if types do not match
*/
Token* parser_advance_expect(Parser* p, TokenType type, const char* msg);

/*
*   [WARNING] HAS ASSERT
*   Check if current token's type matches a type
*/
b32 parser_check(Parser* p, TokenType type);

/*
*   [WARNING] HAS ASSERT
*   Check if next token's type matches a type
*   without incrementing the current index
*/
b32 parser_check_ahead(Parser* p, TokenType type);

/*
*   [WARNING] HAS ASSERT
*   Check if current token's type matches a type
*   and increment the current index
*/
b32 parser_check_advance(Parser* p, TokenType type);

// Top-Level Declarations
AstNode* parse_function_decl(Parser* p);
AstNode* parse_module(Parser* p);
AstNode* parse_import(Parser* p);
AstNode* parse_struct_decl(Parser* p);
AstNode* parse_enum_decl(Parser* p);
AstNode* parse_impl_decl(Parser* p);
AstNode* parse_static_decl(Parser* p);
AstNode* parse_const_decl(Parser* p);

// Statements

AstNode* parse_statement(Parser* p);

/*
*   Returns stack allocated struct, node pointers inside the vec need to be
*   heap allocated. This is meant to be a struct copy directly into
*   the AstNode's AstVec field. Stack copy frees itself after scope 
*   while the node pointers persist on the heap
*/
AstVec parse_block(Parser* p);

/*
*   Returns allocated variable decl node pointer, the value 
*   field is a nullptr if no '=' is present after the identifier.
*/
AstNode* parse_var_decl(Parser* p);

// Expressions

/*
*   Why am I writing these comments like bruh, it parses 
*   an expression TA DA
*/
AstNode* parse_expr(Parser* p);

AstNode* parse_expr_precedence(Parser* p, u32 min_prec);

AstNode* parse_primary(Parser* p);

AstNode* parse_postfix(Parser* p, AstNode* expr);

/*
*   Precendence helper functions
*/

u8 get_precedence(TokenType type); 

b32 is_right_associative(TokenType type); 
b32 is_binary_operator(TokenType type); 
b32 is_assignment_operator(TokenType type); 
b32 is_prefix_operator(TokenType type); 
b32 is_postfix_operator(TokenType type); 

#endif // !AST_PARSER_H
