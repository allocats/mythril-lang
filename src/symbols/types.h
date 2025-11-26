#pragma once
#ifndef SYMBOLS_TYPES_H
#define SYMBOLS_TYPES_H

#include "../ast/types.h"
#include "../type_system/types.h"
#include "../utils/types.h"

#define GENERATE_ENUM(ENUM) ENUM,
#define GENERATE_STRING(STRING) #STRING,

#define FOREACH_SYMBOL_KIND(KIND)   \
    KIND(SYMBOL_VARIABLE)           \
    KIND(SYMBOL_FUNCTION)           \
    KIND(SYMBOL_TYPE)

typedef enum {
    FOREACH_SYMBOL_KIND(GENERATE_ENUM)
} SymbolKind;

typedef struct {
    SymbolKind kind;

    const char* name_ptr;
    usize name_len;
    u64   name_hash;

    Type* type;

    AstNode* node;

    usize scope_depth;

    b8 is_used;
    b8 is_const;

    ssize stack_offset;
} Symbol;

typedef struct Scope Scope;
typedef struct Scope {
    Symbol** symbols;
    usize count;
    usize capacity;

    Scope* parent;
} Scope;

typedef struct SymbolTable {
    Scope* current_scope;
    usize scope_depth;

    ssize current_stack_offset;
} SymbolTable;

#endif // !SYMBOLS_TYPES_H
