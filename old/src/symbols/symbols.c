#include "symbols.h"
#include "types.h"
#include <stdio.h>

SymbolTable* create_symtable(ArenaAllocator* arena) {
    SymbolTable* table = arena_alloc(arena, sizeof(*table));

    table -> current_stack_offset = 0;
    table -> current_scope = nullptr;
    table -> scope_depth = 0;

    return table;
}

Symbol* create_symbol(ArenaAllocator* arena, SymbolKind kind, const char* name, usize len, u64 hash) {
    Symbol* sym = arena_alloc(arena, sizeof(*sym));

    sym -> kind = kind;

    sym -> name_ptr = name;
    sym -> name_len = len;
    sym -> name_hash = hash;

    sym -> type = nullptr;
    sym -> node = nullptr;

    sym -> is_used = false;
    sym -> is_const = false; 

    sym -> stack_offset = 0;

    return sym;
}

Symbol* symbol_lookup(SymbolTable* syms, u64 hash) {
    Scope* scope = syms -> current_scope;

    while (scope) {
        for (usize i = 0; i < scope -> count; i++) {
            Symbol* sym = scope -> symbols[i];

            if (sym -> name_hash == hash) {
                return sym;
            }
        }

        scope = scope -> parent;
    }

    return nullptr;
}

Symbol* symbol_lookup_scope(SymbolTable* syms, u64 hash) {
    Scope* scope = syms -> current_scope;

    if (scope) {
        for (usize i = 0; i < scope -> count; i++) {
            Symbol* sym = scope -> symbols[i];

            if (sym -> name_hash == hash) {
                return sym;
            }
        }
    }

    return nullptr;
}

void define_symbol(ArenaAllocator* arena, SymbolTable* table, Symbol* sym) {
    Symbol* existing = symbol_lookup_scope(table, sym -> name_hash);

    if (existing) {
        // todo: error
        fprintf(stderr, "Error: '%.*s' already defined\n", (i32) sym -> name_len, sym -> name_ptr);
        exit(1);
    }

    sym -> scope_depth = table -> scope_depth;

    Scope* scope = table -> current_scope;

    push_scope(arena, scope, sym);
}

void scope_enter(ArenaAllocator* arena, SymbolTable* table) {
    Scope* new_scope = arena_alloc(arena, sizeof(*new_scope));

    new_scope -> parent = table -> current_scope;
    new_scope -> symbols = arena_alloc(arena, sizeof(Symbol*) * 16);
    new_scope -> capacity = 16;
    new_scope -> count = 0;

    table -> current_scope = new_scope;
    table -> scope_depth++;
}

void scope_exit(SymbolTable* table) {
    if (!table -> current_scope) {
        return;
    }

    Scope* prev_scope = table -> current_scope;

    table -> current_scope = prev_scope -> parent; 
    table -> scope_depth--;
}

void push_scope(ArenaAllocator* arena, Scope* scope, Symbol* symbol) {
    if (scope -> count >= scope -> capacity) {
        usize old_cap = scope -> capacity;
        usize new_cap = old_cap == 0 ? 16 : old_cap * 2;

        usize old_sz = old_cap * sizeof(AstNode*);
        usize new_sz = new_cap * sizeof(AstNode*);

        scope -> symbols = arena_realloc(arena, scope -> symbols, old_sz, new_sz);
        scope -> capacity = new_cap;
    }

    scope -> symbols[scope -> count++] = symbol;
}
