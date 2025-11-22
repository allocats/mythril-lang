#pragma once
#ifndef AST_TYPES_H
#define AST_TYPES_H

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
    AST(AST_IMPORT)         \
    AST(AST_STRUCT)         \
    AST(AST_ENUM)           \
    AST(AST_FUNCTION)       \
    AST(AST_GLOBAL_CONST)   \
    AST(AST_STATIC_VAR)     \
                            \
    AST(AST_VAR_DECL)       \
    AST(AST_CONST_DECL)     \
    AST(AST_ASSIGNMENT)     \
    AST(AST_EXPR_STMT)      \
    AST(AST_FOR)            \
    AST(AST_BLOCK)          \
    AST(AST_RETURN)         \
                            \
    AST(AST_UNARY)          \
    AST(AST_BINARY)         \
    AST(AST_FN_CALL)        \
    AST(AST_LITERAL)        \
    AST(AST_IDENTIFIER)     

typedef enum {
    FOREACH_AST(GENERATE_ENUM)
} AstKind;

static const char* AST_KIND_STRINGS[] = {
    FOREACH_AST(GENERATE_STRING)
};

// Types

#define FOREACH_LITERAL(LIT)  \
    LIT(LIT_UINT)             \
    LIT(LIT_INT)              \
    LIT(LIT_FLOAT)            \
    LIT(LIT_BOOL)             \
    LIT(LIT_STRING)       

typedef enum {
    FOREACH_LITERAL(GENERATE_ENUM)
} LiteralKind;

static const char* LITERAL_STRINGS[] = {
    FOREACH_LITERAL(GENERATE_STRING)
};

#define FOREACH_OP(OP)  \
    OP(OP_ADD)          \
    OP(OP_SUB)          \
    OP(OP_MUL)          \
    OP(OP_DIV)          \
    OP(OP_DEREF)       

typedef enum {
    FOREACH_OP(GENERATE_ENUM)
} OpKind;

static const char* OPKIND_STRINGS[] = {
    FOREACH_OP(GENERATE_STRING)
};

// structs 

typedef struct {
    LiteralKind kind;

    union {
        u64 uint_value;
        i64 int_value;
        f64 float_value;
        b32 bool_value;

        struct {
            char* ptr;
            usize len;
        } string;
    } literal;
} AstLiteral;

typedef struct {
    char* ptr;
    usize len;

    u64 hash;
} AstIdentifier;

typedef struct {
    OpKind op;
    AstNode* operand;
} AstUnary;

typedef struct {
    OpKind op;
    AstNode* left;
    AstNode* right;
} AstBinary;

typedef struct {
    AstNode* identifier;

    AstNode** args;
    usize arg_count;
} AstFnCall;

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
    AstNode** statements;
    usize statement_count;
} AstBlock;

typedef struct {
    AstNode* initial;
    AstNode* condition;
    AstNode* step;
    AstNode* block;
} AstFor;

typedef struct {
    AstNode* type; // just another identifier!
    AstNode* identifier;
    AstNode* value;
} AstAssignment;

typedef struct {
    AstNode* type; // just another identifier!
    AstNode* identifier;
    AstNode* value; // MUST have an expr
} AstConstDecl;

typedef struct {
    AstNode* type; // just another identifier!
    AstNode* identifier;
    AstNode* value; // expr or null
} AstVarDecl;

typedef struct {
    AstNode* return_type; // Just another identifier :3
    AstNode* identifier;

    AstNode** params;
    usize param_count;

    AstNode* block;
} AstFunction;

typedef struct {
    AstNode* identifier;
    AstNode** values;
    usize enum_count;
} AstEnum;

typedef struct {
    AstNode* identifier;

    AstNode** fields;
    usize field_count;

    AstNode** funcs;
    usize func_count;
} AstStruct;

typedef struct {
    AstNode* module;
} AstImport;

typedef struct AstNode {
    usize node_id;

    AstKind kind;

    union {
        // Declarations
        AstImport       import;
        AstStruct       struct_decl;
        AstEnum         enum_decl;
        AstFunction     function;
        
        // Statements
        AstVarDecl      var_decl;
        AstConstDecl    const_decl;
        AstAssignment   assignment;
        AstFor          for_loop;
        AstBlock        block;
        AstReturn       return_stmt;
        
        // Expressions
        AstUnary        unary;
        AstBinary       binary;
        AstFnCall       fn_call;
        AstLiteral      literal;
        AstIdentifier   identifier;
    }; 
} AstNode;

typedef struct {
    AstNode* nodes;
    usize capacity;
    usize count;
} Program;
 
#endif // !AST_TYPES_H
