#pragma once
#ifndef MYTHRIL_SYMBOLS_TYPES_H
#define MYTHRIL_SYMBOLS_TYPES_H

#include "../utils/types.h"

#define X_SYMBOLS(X)    \
    X(SYM_FUNCTION)     \
    X(SYM_VARIABLE)     \
    X(SYM_TYPE)         \

typedef enum {
    X_SYMBOLS(GENERATE_ENUM)
} SymbolKind;

static const char* SYMBOL_KIND_STRINGS[] = {
    X_SYMBOLS(GENERATE_STRING)
};

#define X_SYMBOL_TYPES(X)   \
    X(SYM_TYPE_BASIC)       \
    X(SYM_TYPE_STRUCT)      \
    X(SYM_TYPE_ENUM)        \

typedef enum {
    X_SYMBOL_TYPES(GENERATE_ENUM)
} SymbolTypeKind;

static const char* SYMBOL_TYPE_KIND_STRINGS[] = {
    X_SYMBOL_TYPES(GENERATE_STRING)
};

typedef struct {

} SymbolFunction;

typedef struct {

} SymbolVariable;

typedef struct {

} SymbolType;

typedef struct {
    SymbolKind kind;

    bool defined;

    char* ptr;
    usize len;
    u64 hash;

    usize times_called;

    union {
        SymbolFunction  function;
        SymbolVariable  variable;
        SymbolType      type;
    };
} Symbol;

#endif // !MYTHRIL_SYMBOLS_TYPES_H
