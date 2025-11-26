#pragma once
#ifndef TYPES_SYSTEM_H
#define TYPES_SYSTEM_H

#include "types.h"
#include "types_hashes.h"

#include "../ast/types.h"
#include "../semantics/types.h"

Type* create_type(ArenaAllocator* arena, TypeKind kind, usize size, usize align);
Type* resolve_type(SemanticCtx* ctx, AstNode* node);

b32 type_is_signed(Type* a);
b32 type_is_unsigned(Type* a);

b32 type_is_integer(Type* a);
b32 type_is_float(Type* a);

b32 type_is_number(Type* a);

b32 types_equal(Type* a, Type* b);
b32 types_compatible(Type* a, Type* b);

#endif // !TYPES_SYSTEM_H
