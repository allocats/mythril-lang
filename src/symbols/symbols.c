#include "symbols.h"
#include "types.h"

#include <stdio.h>

#define DEFAULT_CAP 64

[[gnu::always_inline]]
u32 hash_fnv1a(const char* s, u32 len) {
    u32 hash = 2166136261u;

    for (u32 i = 0; i < len; i++) {
        hash ^= s[i];
        hash *= 16777619u;
    }

    return hash;
} 

SymbolTable* enter_scope(
    ArenaAllocator* arena,
    SymbolTable* current
) {
    SymbolTable* new_scope = arena_alloc(arena, sizeof(*new_scope));

    new_scope -> parent = current;
    new_scope -> symbols = arena_alloc(arena, sizeof(Symbol) * DEFAULT_CAP);
    new_scope -> capacity = DEFAULT_CAP;
    new_scope -> count = 0;
    new_scope -> scope_depth = current ? current -> scope_depth + 1 : 0;
    
    return new_scope;
}

[[gnu::always_inline]]
SymbolTable* exit_scope(
    SymbolTable* current
) {
    return current -> parent;
}

void add_symbol(
    SymbolTable* table,
    Symbol symbol
) {
    for (usize i = 0; i < table -> count; i++) {
        if (table -> symbols[i].hash == symbol.hash) {
            fprintf(
                stderr,
                "Symbol '%.*s' already defined",
                symbol.name_len,
                symbol.name_ptr
            );
            exit(1);
        }
    }

    table -> symbols[table -> count++] = symbol;
}

Symbol* lookup_symbol_all(
    SymbolTable* table,
    u32 hash
) {
    SymbolTable* curr = table;

    while (curr) {
        for (usize i = 0; i < curr -> count; i++) {
            if (curr -> symbols[i].hash == hash) {
                return &(curr -> symbols[i]);
            }
        }

        curr = exit_scope(curr);
    }

    return nullptr;
}

Symbol* lookup_symbol_scope(
    SymbolTable* table,
    u32 hash
) {
    for (usize i = 0; i < table -> count; i++) {
        if (table -> symbols[i].hash == hash) {
            return &(table -> symbols[i]);
        }
    }

    return nullptr;
}
