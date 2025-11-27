#pragma once
#ifndef AST_TYPES_H
#define AST_TYPES_H

#include "../arena/arena.h"
#include "../token/types.h"
#include "../utils/types.h"

// what is a program? what is source code?

typedef struct AstNode AstNode;

#define GENERATE_ENUM(ENUM) ENUM,
#define GENERATE_STRING(STRING) #STRING,

/*
*
*   1. Declarations
*   2. Statements
*   3. Expressions
*
*/

#define FOREACH_AST(AST)    \
    AST(AST_MODULE)         \
    AST(AST_IMPORT)         \
    AST(AST_MACRO)          \
    AST(AST_STRUCT)         \
    AST(AST_ENUM)           \
    AST(AST_IMPL)           \
    AST(AST_FUNCTION)       \
    AST(AST_GLOBAL_CONST)   \
    AST(AST_STATIC_VAR)     \
                            \
    AST(AST_VAR_DECL)       \
    AST(AST_CONST_DECL)     \
    AST(AST_ASSIGNMENT)     \
    AST(AST_EXPR_STMT)      \
    AST(AST_IF)             \
    AST(AST_FOR)            \
    AST(AST_RETURN)         \
                            \
    AST(AST_INDEX)          \
    AST(AST_MEMBER_ACCESS)  \
    AST(AST_UNARY)          \
    AST(AST_BINARY)         \
    AST(AST_FN_CALL)        \
    AST(AST_LITERAL)        \
    AST(AST_POSTFIX)        \
    AST(AST_IDENTIFIER)     

typedef enum {
    FOREACH_AST(GENERATE_ENUM)
} AstKind;

static const char* AST_KIND_STRINGS[] = {
    FOREACH_AST(GENERATE_STRING)
};

// Types

#define FOREACH_LITERAL(LIT)    \
    LIT(LIT_UINT)               \
    LIT(LIT_INT)                \
    LIT(LIT_FLOAT)              \
    LIT(LIT_BOOL)               \
    LIT(LIT_STRING)             \
    LIT(LIT_NULL)       

typedef enum {
    FOREACH_LITERAL(GENERATE_ENUM)
} LiteralKind;

static const char* LITERAL_STRINGS[] = {
    FOREACH_LITERAL(GENERATE_STRING)
};

/*
*
* EQ    = equals
* NEQ   = not equals
* LT    = less than
* LTE   = less than or equals to
* GT    = greater than
* GTE   = greater than or equals to
* ADDR  = address (&var)
* DEREF = derefence
*
*/
#define FOREACH_OP(OP)  \
    OP(OP_ADD)          \
    OP(OP_SUB)          \
    OP(OP_MUL)          \
    OP(OP_DIV)          \
    OP(OP_MOD)          \
    OP(OP_EQ)           \
    OP(OP_NEQ)          \
    OP(OP_LT)           \
    OP(OP_LTE)          \
    OP(OP_GT)           \
    OP(OP_GTE)          \
    OP(OP_ADDR)         \
    OP(OP_DEREF)      

typedef enum {
    FOREACH_OP(GENERATE_ENUM)
} OpKind;

static const char* OPKIND_STRINGS[] = {
    FOREACH_OP(GENERATE_STRING)
};

// structs 

typedef struct {
    AstNode** nodes;
    usize count;
    usize cap;
} AstVec;

typedef struct {
    LiteralKind kind;

    // Store the string representation, convert during semantics, or just work with it entirely
    char* ptr;
    usize len;
} AstLiteral;

typedef struct {
    TokenType op;
    AstNode* operand;
} AstPostfix;

typedef struct {
    const char* ptr;
    usize len;

    u64 hash;
} AstIdentifier;

typedef struct {
    // OpKind op;
    TokenType op;
    AstNode* operand;
} AstUnary;

typedef struct {
    // OpKind op;
    TokenType op;
    AstNode* left;
    AstNode* right;
} AstBinary;

typedef struct {
    AstNode* identifier;
    AstVec args;
} AstFnCall;

typedef struct {
    AstNode* array;
    AstNode* index;
} AstIndex;

typedef struct {
    AstNode* object;

    char* member_ptr;
    usize member_len;
    u64   member_hash;
} AstMemberAccess;

typedef struct {
    AstKind kind;
    
    union {
        AstUnary        unary;
        AstBinary       binary;
        AstFnCall       fn_call;
        AstLiteral      literal;
        AstIdentifier   identifier;
    } expr;
} AstExpr;

typedef struct {
    AstNode* expression;
} AstReturn;

typedef struct {
    AstNode* initial;
    AstNode* condition;
    AstNode* step;
    AstVec   block;
} AstFor;

typedef struct {
    AstNode* cond;
    AstVec if_block;
    AstVec else_block;
} AstIf;

typedef struct {
    // AstNode* type; // just another identifier!
    AstNode* target;
    AstNode* value;
    OpKind op;
} AstAssignment;

typedef struct {
    AstNode* type; // just another identifier!
    AstNode* identifier;
    AstNode* value; // MUST have an expr
} AstConstDecl;

typedef struct {
    AstNode* type; // just another identifier!
    usize pointer_depth;
    AstNode* identifier;
    AstNode* value; // expr or null
} AstVarDecl;

typedef struct {
    AstNode* return_type; // Just another identifier :3
    AstNode* identifier;

    AstVec params;

    AstVec block;
} AstFunction;

typedef struct {
    AstNode* identifier;
    AstNode** values;
    usize enum_count;
} AstEnum;

typedef struct {
    AstNode* identifier;

    AstVec fields;
} AstStruct;

typedef struct {
    AstNode* type_name;
    AstVec functions;
} AstImpl;

// Starting off with C like macros, text replacement
typedef struct {
    AstNode* identifier;
    AstVec params;
    AstVec block;
} AstMacro;

typedef struct {
    AstNode* module;
} AstImport;

typedef struct {
    AstNode* identifier;
} AstModule;

typedef struct AstNode {
    AstKind kind;

    union {
        // Declarations
        AstModule       module;
        AstImport       import;
        AstMacro        macro_decl;
        AstStruct       struct_decl;
        AstEnum         enum_decl;
        AstImpl         impl;
        AstFunction     function;
        
        // Statements
        AstVarDecl      var_decl;
        AstConstDecl    const_decl;
        AstAssignment   assignment;
        AstIf           if_stmt;
        AstFor          for_loop;
        AstReturn       return_stmt;
        
        // Expressions
        AstUnary        unary;
        AstBinary       binary;
        AstFnCall       fn_call;
        AstLiteral      literal;
        AstIdentifier   identifier;
        AstPostfix      postfix;
        AstIndex        index;
        AstMemberAccess member_access;
    }; 
} AstNode;

typedef struct {
    ArenaAllocator* arena;
    AstNode* nodes;
    usize capacity;
    usize count;
    b32 had_error;
} Program;
 
#endif // !AST_TYPES_H
