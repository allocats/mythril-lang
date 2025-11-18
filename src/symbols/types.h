#pragma once
#ifndef SYMBOLS_TYPES_H
#define SYMBOLS_TYPES_H

#include "../ast/types.h"
#include "../token/types.h"
#include "../utils/types.h"

// Symbol type

#define FOREACH_SYMBOL(SYM) \
    SYM(S_FUNC)             \
    SYM(S_PARAM)            \
    SYM(S_STRUCT)           \
    SYM(S_CONST)            \
    SYM(S_VAR)              

#define GENERATE_ENUM(ENUM) ENUM,
#define GENERATE_STRING(STRING) #STRING,

typedef enum {
    FOREACH_SYMBOL(GENERATE_ENUM)
} SymbolType;

static const char* SYMBOL_TYPES_STRINGS[] = {
    FOREACH_SYMBOL(GENERATE_STRING)
};

// Symbol scope

#define FOREACH_SCOPE(SCOPE)\
    SCOPE(SC_GLOBAL)        \
    SCOPE(SC_LOCAL)         

typedef enum {
    FOREACH_SCOPE(GENERATE_ENUM)
} SymbolScope;

static const char* SYMBOL_SCOPE_STRINGS[] = {
    FOREACH_SCOPE(GENERATE_STRING)
};

// ****

typedef struct {
    char* type_ptr;
    usize type_len;
    u32 pointer_depth;
} TypeInfo;

typedef struct {
    TypeInfo return_type;

    TypeInfo* param_types;
    usize param_count;

    b32 is_defined;
    b32 is_builtin;

    AstNode* block;
} FunctionSymbol;

typedef struct {
    TypeInfo* field_types;
    usize* field_offsets;
    usize field_count;

    char** field_name_ptrs;
    usize* field_name_lens;

    usize total_size; // my_struct.size() :p
} StructSymbol;

typedef struct {
    TypeInfo type;
    union {
        i32 int_value;
        f64 float_value;

        struct {
            char* ptr;
            usize len;
        } string_value;
    } value;
} ConstSymbol;

typedef struct {
    TypeInfo info;
    usize stack_offset;
    b32 is_initialized;
} VarSymbol;

typedef struct {
    u32 hash;

    u32 name_len;
    char* name_ptr;

    SymbolType type;
    SymbolScope scope;

    usize defined_at_line;

    union {
        FunctionSymbol  function;
        ConstSymbol     constant;
        VarSymbol       variable;
        VarSymbol       parameter;
        StructSymbol    structure;
    } data;
} Symbol;

typedef struct SymbolTable SymbolTable;

struct SymbolTable {
    SymbolTable* parent; // nullptr for global scopes
    Symbol* symbols;
    usize capacity;
    usize count;
    usize scope_depth; // 0 = global, 1 or more = nested
};

#endif // !SYMBOLS_TYPES_H
