#pragma once
#ifndef SYMBOLS_H
#define SYMBOLS_H

#include "types.h"

#include "../arena/arena.h"

SymbolTable* create_symtable(ArenaAllocator* arena);

Symbol* create_symbol(ArenaAllocator* arena, SymbolKind kind, const char* name, usize len, u64 hash);

Symbol* symbol_lookup(SymbolTable* syms, u64 hash);
Symbol* symbol_lookup_scope(SymbolTable* syms, u64 hash);

void define_symbol(ArenaAllocator* arena, SymbolTable* table, Symbol* sym);

void scope_enter(ArenaAllocator* arena, SymbolTable* table);
void scope_exit(SymbolTable* table);

void push_scope(ArenaAllocator* arena, Scope* scope, Symbol* symbol);

#endif // !SYMBOLS_H
