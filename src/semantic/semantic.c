#include "semantic.h"

#include "../symbols/symbols.h"

#include <stdbool.h>
#include <stdio.h>

void semantic_analyze_function(
    AstFunction* func,
    ArenaAllocator* arena,
    SymbolTable* global_table
) {
    Symbol symbol = {
        .type = S_FUNC,
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

        char* type_ptr = func -> params[i] -> as.var_decl.type_ptr;
        usize type_len = func -> params[i] -> as.var_decl.type_len;

        type -> type_ptr = type_ptr;
        type -> type_len = type_len;
    }

    u32 hash = hash_fnv1a(symbol.name_ptr, symbol.name_len);

    Symbol* sym = lookup_symbol_all(global_table, hash);

    if (!sym) {
        add_symbol(global_table, symbol);
        return;
    }

    if (sym -> data.function.is_defined == false && symbol.data.function.is_defined == true) {
        sym -> data.function.is_defined = true;
        sym -> data.function.block = symbol.data.function.block;
        return;
    }

    fprintf(stderr, "Function '%.*s' already defined", symbol.name_len, symbol.name_ptr);
    exit(1);
}
