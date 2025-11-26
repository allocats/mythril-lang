#include "primitives.h"
#include "type_system.h"
#include "types_hashes.h"
#include "types.h"

#include "../arena/arena.h"
#include "../symbols/symbols.h"

#include <stdio.h>

Type* create_type(ArenaAllocator* arena, TypeKind kind, usize size, usize align) {
    Type* type = arena_alloc(arena, sizeof(*type));

    type -> kind = kind;
    type -> size = size;
    type -> align = align;

    return type;
}

Type* resolve_type(SemanticCtx* ctx, AstNode* node) {
    if (node -> kind != AST_IDENTIFIER) {
        // error
        exit(1);
    }

    u64 hash = node -> identifier.hash;

    switch (hash) {
        case VOID_HASH: {
            return &type_void;
        } break;

        case BOOL_HASH: {
            return &type_bool;
        } break;

        case USIZE_HASH: {
            return &type_usize;
        } break;

        case SSIZE_HASH: {
            return &type_ssize;
        } break;

        case UINT8_HASH: {
            return &type_uint8;
        } break;

        case UINT16_HASH: {
            return &type_uint16;
        } break;

        case UINT32_HASH: {
            return &type_uint32;
        } break;

        case UINT64_HASH: {
            return &type_uint64;
        } break;

        case INT8_HASH: {
            return &type_int8;
        } break;

        case INT16_HASH: {
            return &type_int16;
        } break;

        case INT32_HASH: {
            return &type_int32;
        } break;

        case INT64_HASH: {
            return &type_int64;
        } break;

        case F32_HASH: {
            return &type_f32;
        } break;

        case F64_HASH: {
            return &type_f64;
        } break;

        default: {
            break;
        }
    }

    Symbol* sym = symbol_lookup(ctx -> symbols, hash);

    if (sym && sym -> kind == SYMBOL_TYPE) {
        return sym -> type;
    }

    // error
    fprintf(stderr, "Error: Unknown type\n");
    exit(1);
} 

b32 type_is_signed(Type* a) {
    return (a -> kind == TYPE_SSIZE) || (a -> kind >= TYPE_INT8 && a -> kind <= TYPE_INT64);
}

b32 type_is_unsigned(Type* a) {
    return (a -> kind == TYPE_USIZE) || (a -> kind >= TYPE_UINT8 && a -> kind <= TYPE_UINT64);
}

b32 type_is_integer(Type* a) {
    return a -> kind >= TYPE_USIZE && a -> kind <= TYPE_UINT64;
}

b32 type_is_float(Type* a) {
    return a -> kind == TYPE_F32 || a -> kind == TYPE_F64;
}

b32 type_is_number(Type* a) {
    return type_is_integer(a) || type_is_float(a);
}

b32 types_equal(Type* a, Type* b) {
    if (a == b) {
        return true;
    }

    if (a -> kind != b -> kind) {
        return false;
    }

    if (a -> kind == TYPE_POINTER) {
        return types_equal(a -> pointer, b -> pointer);
    }

    return false;
}

b32 types_compatible(Type* a, Type* b) {
    if (types_equal(a, b)) {
        return true;
    }

    // promoting ints
    if (type_is_integer(a) && type_is_integer(b)) {
        if (type_is_unsigned(a) == type_is_unsigned(b)) {
            return a -> size <= b -> size;
        }
    }

    return false;
}
