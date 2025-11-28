#pragma once
#ifndef MYTHRIL_AST_TYPES_H
#define MYTHRIL_AST_TYPES_H

#include "../utils/types.h"

#define X_AST(X)        \
    X(AST_MODULE_DECL)  \
    X(AST_IMPORT_DECL)  \
    X(AST_STRUCT_DECL)  \
    X(AST_ENUM_DECL)    \
    X(AST_IMPL_DECL)    \
    X(AST_FUNCTION_DECL)\
    X(AST_CONST_DECL)   \

typedef enum {
    X_AST(GENERATE_ENUM)
} AstKind;

static const char* AST_KIND_STRINGS[] = {
    X_AST(GENERATE_STRING)
};

typedef struct AstNode AstNode;

typedef struct {
    AstNode* identifier;
} AstModuleDecl;

typedef struct {
    AstNode* identifier;
} AstImportDecl;

typedef struct AstNode {
    AstKind kind;

    union {
        AstModuleDecl   module_decl;
    };
} AstNode;

#endif // !MYTHRIL_AST_TYPES_H
