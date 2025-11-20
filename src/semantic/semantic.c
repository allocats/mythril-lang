#include "semantic.h"

#include "../symbols/symbols.h"

#include <stdbool.h>
#include <stdio.h>
#include <string.h>

/*
*
*   proto
*
*/

void semantic_analyze_function(
    AstFunction* func,
    ArenaAllocator* arena,
    SymbolTable* global_table
);

void semantic_analyze_block(
    AstFunction* func,
    Symbol* symbol
);

/*
*
*   defs
*
*/

static void error_already_defined(
    u32 len,
    const char* s
) {
    fprintf(stderr, "Error: '%.*s' is already defined\n", len, s);
    exit(1);
}

static void error_already_defined_as(
    u32 len,
    const char* s,
    const char* type
) {
    fprintf(stderr, "Error: '%.*s' is already defined as a %s\n", len, s, type);
    exit(1);
}

static void error_type_mismatch(
    u32 expected_len,
    const char* expeted_ptr,
    u32 found_len,
    const char* found_ptr 
) {
    fprintf(
        stderr,
        "Error: Expected '%.*s' found '%.*s'\n",
        expected_len,
        expeted_ptr,
        found_len,
        found_ptr
    );
    exit(1);
}

void semantic_analyze_program(
    Program *program,
    ArenaAllocator *arena,
    SymbolTable *global_table
) {
    for (usize i = 0; i < program -> count; i++) {
        AstNode* node = &program -> items[i];

        switch (node -> type) {
            case A_FUNC: {
                semantic_analyze_function(&node -> ast.function, arena, global_table);
            } break;

            default: {
                fprintf(stderr, "erm");
                exit(1);
            } break;
        }
    }
}

void semantic_analyze_function(
    AstFunction* func,
    ArenaAllocator* arena,
    SymbolTable* global_table
) {
    Symbol symbol = {
        .sym_type = S_FUNC,
        .scope = SC_GLOBAL,
        .name_ptr = func -> name_ptr,
        .name_len = func -> name_len,

        .data.function = {
            .param_types = arena_alloc(arena, sizeof(TypeInfo) * func -> param_count),
            .param_count = func -> param_count,
            .is_builtin = false,
            .return_type = {
                .type_ptr = func -> ret_ptr, 
                .type_len = func -> ret_len, 
                .pointer_depth = 0
            }
        }
    };

    symbol.data.function.block = func -> block;

    if (func -> block) {
        symbol.data.function.is_defined = true;
    }

    for (usize i = 0; i < func -> param_count; i++) {
        TypeInfo* type = &(symbol.data.function.param_types[i]);

        char* type_ptr = func -> params[i] -> ast.var_decl.type_ptr;
        usize type_len = func -> params[i] -> ast.var_decl.type_len;

        type -> type_ptr = type_ptr;
        type -> type_len = type_len;
    }

    u64 hash = hash_fnv1a(symbol.name_ptr, symbol.name_len);

    symbol.hash = hash;

    Symbol* sym = lookup_symbol_all(global_table, hash);

    if (sym && sym -> hash != 0) {
        if (sym -> sym_type != S_FUNC) {
            error_already_defined_as(
                sym -> name_len,
                sym -> name_ptr,
                SYMBOL_TYPES_STRINGS[sym -> sym_type]
            );
        }

        if (!sym -> data.function.is_defined && symbol.data.function.is_defined) {
            TypeInfo* type_1 = &sym -> data.function.return_type;
            TypeInfo* type_2 = &symbol.data.function.return_type;

            if (memcmp(type_1, type_2, sizeof(*type_1)) != 0 ) {
                error_type_mismatch(
                    type_1 -> type_len,
                    type_1 -> type_ptr,
                    type_2 -> type_len,
                    type_2 -> type_ptr
                );
            }

            sym -> data.function.is_defined = true;
            sym -> data.function.block = symbol.data.function.block;
            return;
        }

        if (sym -> data.function.is_defined && symbol.data.function.is_defined) {
            error_already_defined(
                sym -> name_len,
                sym -> name_ptr
            );
        }

        return;
    }

    add_symbol(global_table, symbol);

    // semantic_analyze_block(func, &global_table -> symbols[global_table -> count - 1]);
}
