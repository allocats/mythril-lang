#pragma once
#ifndef SYMBOLS_H
#define SYMBOLS_H

#include "types.h"

u64 hash_fnv1a(const char* s, u32 len);

SymbolTable* enter_scope(ArenaAllocator* arena, SymbolTable* current);
SymbolTable* exit_scope(SymbolTable* current);

void add_symbol(SymbolTable* table, Symbol symbol);

Symbol* lookup_symbol_all(SymbolTable* table, u64 hash);
Symbol* lookup_symbol_scope(SymbolTable* table, u64 hash);

#endif // !SYMBOLS_H
