#pragma once
#ifndef AST_H
#define AST_H

#include "types.h"

#include "../arena/arena.h"

Program* ast_build(Tokens* tokens, ArenaAllocator* arena);

/*
*
*   helper creation functions
*
*/

AstNode* ast_create_func(
    AstNode* node,
    char* name_ptr,
    usize name_len,
    char* ret_ptr,
    usize ret_len,
    AstNode** params,
    usize param_count,
    AstNode* block
); 

AstNode* ast_create_var_decl(
    ArenaAllocator* arena,
    char* type_ptr,
    usize type_len,
    u32 pointer_depth,
    char* name_ptr,
    usize name_len,
    AstNode* initializer 
); 

AstNode* ast_create_func_call(
    ArenaAllocator* arena,
    char* name_ptr,
    usize name_len,
    AstNode** args,
    usize arg_count
); 

AstNode* ast_create_block(
    ArenaAllocator* arena,
    AstNode** statements,
    usize count
); 

AstNode* ast_create_identifier(
    ArenaAllocator* arena,
    char* name_ptr,
    usize name_len
); 

AstNode* ast_create_number_literal(
    ArenaAllocator* arena,
    u64 value 
); 

AstNode* ast_create_string_literal(
    ArenaAllocator* arena,
    char* ptr,
    u32 len
); 

AstNode* ast_create_return(
    ArenaAllocator* arena,
    AstNode* expr
); 

AstNode* ast_create_assignment(
    ArenaAllocator* arena,
    AstNode* target,
    AstNode* value 
); 

AstNode* ast_create_binary_expr(
    ArenaAllocator* arena,
    TokenType op_type,
    AstNode* left,
    AstNode* right
);

/*
*
*   print functions
*
*/

void print_indent(int depth); 
void print_slice(char* ptr, usize len); 
void print_function(AstFunction* func, int depth); 
void print_block(AstBlock* block, int depth); 
void print_var_decl(AstVarDecl* decl, int depth); 
void print_assignment(AstAssignment* assign, int depth); 
void print_literal(AstLiteral* lit, int depth); 
void print_identifier(AstIdentifier* ident, int depth); 
void print_function_call(AstFnCall* call, int depth); 
void print_return(AstReturn* ret, int depth); 
void print_node(AstNode* node, int depth); 
void print_program(Program* funcs); 

#endif // !AST_H
