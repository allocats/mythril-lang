#pragma once
#ifndef AST_TYPES_H
#define AST_TYPES_H

#include "../arena/arena.h"
#include "../token/types.h"
#include "../utils/types.h"

typedef struct AstNode AstNode;

#define FOREACH_AST(AST)    \
    AST(A_FUNC)             \
    AST(A_BLOCK)            \
    AST(A_FUNC_CALL)        \
    AST(A_RETURN)           \
    AST(A_EXPR)             \
                            \
    AST(A_VAR_DECL)         \
    AST(A_ASSIGNMENT)       \
    AST(A_IDENTIFIER)       \
    AST(A_LITERAL) 

#define GENERATE_ENUM(ENUM) ENUM,
#define GENERATE_STRING(STRING) #STRING,

typedef enum {
    FOREACH_AST(GENERATE_ENUM)
} AstType;

static const char* AST_TYPES_STRINGS[] = {
    FOREACH_AST(GENERATE_STRING)
};

#define FOREACH_EXPR_KIND(KIND) \
    KIND(EXPR_UNARY)            \
    KIND(EXPR_BINARY) 

typedef enum {
    FOREACH_EXPR_KIND(GENERATE_ENUM)
} ExprKind;

static const char* EXPR_KIND_STRINGS[] = {
    FOREACH_EXPR_KIND(GENERATE_STRING)
};

typedef struct {
    char* ret_ptr;
    usize ret_len;

    char* name_ptr;
    usize name_len;

    AstNode** params;
    usize param_count;

    AstNode* block;
} AstFunction;

typedef struct {
    AstNode** statements;
    usize count;
} AstBlock;

typedef struct {
    u32 pointer_depth;

    char* type_ptr;
    usize type_len;

    char* name_ptr;
    usize name_len;

    AstNode* initializer;   // points to assignment node or is a nullptr
} AstVarDecl;

typedef struct {
    AstNode* target;    // identifier 
    AstNode* value;     // can be an expression
} AstAssignment;

typedef struct {
    TokenType type;

    union {
        b32 bool_value;
        i64 int_value;
        u64 uint_value;
        f64 float_value;

        struct {
            char* ptr;
            u32 len;
        } string;
    } value;
} AstLiteral;

typedef struct {
    char* name_ptr;
    usize name_len;
} AstIdentifier;

typedef struct {
    char* name_ptr;
    usize name_len;

    AstNode** args;
    usize arg_count;
} AstFnCall;

typedef struct {
    AstNode* expression;
} AstReturn;

typedef struct {
    ExprKind kind;

    union {
        struct {
            TokenType op_kind;
            AstNode* operand;
        } unary;

        struct {
            TokenType op_kind;
            AstNode* left;
            AstNode* right;
        } binary;
    } expr;
} AstExpr;

struct AstNode{
    AstType type;

    union {
        AstFunction     function;
        AstBlock        block;
        AstReturn       ret;
        AstFnCall       function_call;
        AstVarDecl      var_decl;
        AstAssignment   assignment;
        AstLiteral      literal;
        AstIdentifier   identifier;
        AstExpr         expression;
    } ast;
};

typedef struct {
    AstNode* items;
    usize capacity;
    usize count;
} Program;

typedef struct {
    ArenaAllocator* arena;
    Tokens* tokens;
    usize count;
    usize index;
} Parser;

#endif // !AST_TYPES_H
