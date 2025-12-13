#pragma once
#ifndef MYTHRIL_AST_TYPES_H
#define MYTHRIL_AST_TYPES_H

#include "../tokens/types.h"
#include "../utils/types.h"

#define X_AST(X)            \
    /* declarations */      \
    X(AST_MODULE_DECL)      \
    X(AST_IMPORT_DECL)      \
    X(AST_STRUCT_DECL)      \
    X(AST_ENUM_DECL)        \
    X(AST_IMPL_DECL)        \
    X(AST_FUNCTION_DECL)    \
    X(AST_STATIC_DECL)      \
    X(AST_CONST_DECL)       \
    X(AST_VAR_DECL)         \
                            \
    /* statements */        \
    X(AST_ASSIGNMENT)       \
    X(AST_IF_STMT)          \
    X(AST_MATCH_STMT)       \
    X(AST_LOOP_STMT)        \
    X(AST_FOR_STMT)         \
    X(AST_WHILE_STMT)       \
    X(AST_BREAK_STMT)       \
    X(AST_CONTINUE_STMT)    \
    X(AST_RETURN_STMT)      \
    X(AST_EXPR_STMT)        \
                            \
    /* expressions */       \
    X(AST_UNARY)            \
    X(AST_BINARY)           \
    X(AST_FUNCTION_CALL)    \
    X(AST_ARRAY_INDEX)      \
    X(AST_MEMBER_ACCESS)    \
    X(AST_IDENTIFIER)       \
    X(AST_LITERAL)          \
                            \
    /* patterns */          \
    X(AST_PATTERN_IDENT)    \
    X(AST_PATTERN_LITERAL)  \
    X(AST_PATTERN_VARIANT)  \
    X(AST_PATTERN_WILDCARD) \
                            \
    X(AST_ERROR) /* set node to error if error is present*/

typedef enum {
    X_AST(GENERATE_ENUM)
} AstKind;

static const char* AST_KIND_STRINGS[] = {
    X_AST(GENERATE_STRING)
};

#define X_TYPES(X)  \
    X(TYPE_ERR)     \
    X(TYPE_BASIC)   \
    X(TYPE_POINTER) \
    X(TYPE_ARRAY)

typedef enum {
    X_TYPES(GENERATE_ENUM)
} TypeKind;

static const char* TYPE_KIND_STRINGS[] = {
    X_TYPES(GENERATE_STRING)
};

typedef struct AstNode AstNode;

typedef struct {
    const char* ptr;
    usize len;
    u64 hash;
} AstSlice;

typedef struct AstType AstType;

typedef struct AstType {
    TypeKind kind;
    
    union {
        // basic
        AstSlice identifier;

        // pointer
        AstType* pointee;

        // array
        struct {
            AstType* element_type;
            AstNode* size_expr;
        } array;
    };
} AstType;

typedef struct {
    AstSlice* segments;
    usize count;
} AstModuleDecl;

typedef struct {
    AstSlice* segments;
    usize count;
} AstImportDecl;

typedef struct {
    AstSlice identifier;
    AstType* type;
} AstStructField;

typedef struct {
    AstSlice identifier;

    AstStructField* fields;
    usize capacity;
    usize count;
} AstStructDecl;

typedef struct {
    AstSlice identifier;

    AstType** types;
    usize capacity;
    usize count;
} AstEnumVariant;

typedef struct {
    AstSlice identifier;

    AstEnumVariant** variants;
    usize capacity;
    usize count;
} AstEnumDecl;

typedef struct {
    AstSlice target;

    AstNode** functions;
    usize fn_capacity;
    usize fn_count;
} AstImplDecl;

typedef struct {
    AstSlice identifier;
    AstType* type;
} AstParameter;

typedef struct {
    AstSlice identifier;

    AstParameter* parameters;
    usize param_capacity;
    usize param_count;

    AstType* return_type;

    // block
    AstNode** statements;
    usize stmt_capacity;
    usize stmt_count;
} AstFunctionDecl;

typedef struct {
    AstSlice identifier;
    AstType* type;

    // cannot be nullptr
    AstNode* value;
} AstConstDecl;

typedef struct {
    AstSlice identifier;
    AstType* type;

    AstNode* value;
} AstVarDecl;

typedef struct {
    AstSlice identifier;
    AstType* type;

    AstNode* value;
} AstStaticDecl;

typedef struct {
    AstNode* lvalue;

    TokenKind op;

    AstNode* rvalue;
} AstAssignment;

typedef struct {
    AstNode* expression;
    
    // block
    AstNode** statements;
    usize stmt_capacity;
    usize stmt_count;

    AstNode* else_stmt;
} AstIfStmt;

typedef struct {
    AstNode* pattern;

    // block
    AstNode** statements;
    usize stmt_capacity;
    usize stmt_count;
} AstMatchArm;

typedef struct {
    AstNode* expression;

    AstMatchArm* arms;
    usize arm_capacity;
    usize arm_count;

    // block
    // "_: { ... }"
    AstNode** statements;
    usize stmt_capacity;
    usize stmt_count;
} AstMatchStmt;

typedef struct {
    // block
    AstNode** statements;
    usize stmt_capacity;
    usize stmt_count;
} AstLoopStmt;

typedef struct {
    AstNode* cond;

    // block
    AstNode** statements;
    usize stmt_capacity;
    usize stmt_count;
} AstWhileStmt;

typedef struct {
    AstNode* init;
    AstNode* cond;
    AstNode* step;

    // block
    AstNode** statements;
    usize stmt_capacity;
    usize stmt_count;
} AstForStmt;

typedef struct {
} AstBreakStmt;

typedef struct {
} AstContinueStmt;

typedef struct {
    AstNode* expression;
} AstReturnStmt;

typedef struct {
    AstNode* expression;
} AstExprStmt;

typedef struct {
    TokenKind op;
    AstNode* operand;
} AstUnary;

typedef struct {
    TokenKind op;
    AstNode* left;
    AstNode* right;
} AstBinary;

typedef struct {
    AstSlice identifier;
    AstNode** arguments;
    usize arg_capacity;
    usize arg_count;
} AstFunctionCall;

typedef struct {
    AstNode* array;
    AstNode* index;
} AstArrayIndex;

typedef struct {
    AstNode* object;
    TokenKind op;
    AstSlice member;
} AstMemberAccess;

typedef struct {
    AstSlice value;
} AstIdentifier;

typedef struct {
    TokenKind kind;
    AstSlice value;
} AstLiteral;

typedef struct {
    AstSlice identifier;
} AstPatternIdent;

typedef struct {
    AstLiteral literal;
} AstPatternLiteral;

typedef struct {
    AstSlice variant;

    AstNode** patterns;
    usize capacity;
    usize count;
} AstPatternVariant;

typedef struct AstNode {
    AstKind kind;

    union {
        /* declarations */
        AstModuleDecl       module_decl;
        AstImportDecl       import_decl;
        AstStructDecl       struct_decl;
        AstEnumDecl         enum_decl;
        AstImplDecl         impl_decl;
        AstFunctionDecl     function_decl;
        AstStaticDecl       static_decl;
        AstConstDecl        const_decl;
        AstVarDecl          var_decl;
        
        /* statements */
        AstAssignment       assignment;
        AstIfStmt           if_stmt;
        AstMatchStmt        match_stmt;
        AstLoopStmt         loop_stmt;
        AstWhileStmt        while_stmt;
        AstForStmt          for_stmt;
        AstBreakStmt        break_stmt;
        AstContinueStmt     continue_stmt;
        AstExprStmt         expr_stmt;
        AstReturnStmt       return_stmt;
        
        /* expressions */
        AstBinary           binary;
        AstUnary            unary;
        AstFunctionCall     function_call;
        AstArrayIndex       array_index;
        AstMemberAccess     member_access;
        AstIdentifier       identifier;
        AstLiteral          literal;
        
        /* patterns */
        AstPatternIdent     pattern_ident;
        AstPatternLiteral   pattern_literal;
        AstPatternVariant   pattern_variant;
    };
} AstNode;

typedef struct {
    AstNode** declarations;
    usize capacity;
    usize count;
} Program;

#endif // !MYTHRIL_AST_TYPES_H
