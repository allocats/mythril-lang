#pragma once
#ifndef MYTHRIL_AST_PARSER_TYPES_H
#define MYTHRIL_AST_PARSER_TYPES_H

#include "../arena/arena.h"
#include "../ast/types.h"
#include "../diagnostics/types.h"
#include "../tokens/types.h"
#include "../utils/types.h"

#define DELIMITER_STACK_MAX 128

typedef struct {
    Token* token;

    /* function name, struct name, etc. might remove this */
    const char* context;
} Delimiter;

typedef struct {
    i32 top;
    u8 _padding[4];
    Delimiter items[DELIMITER_STACK_MAX];
} DelimiterStack;

typedef struct {
    ArenaAllocator* arena;
    Tokens* tokens;
    Program* program;

    const char* path;

    usize index;
    usize count;

    // impl "Foo"; fn "foo" store the identifier of current contexts
    // for better error handling, can now say:
    // "error at function foo in implementation of Foo"
    const char* current_fn;
    const char* current_struct;

    DelimiterStack delimiters;
} Parser;

#endif // !MYTHRIL_AST_PARSER_TYPES_H
