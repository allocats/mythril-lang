#pragma once
#ifndef TYPES_SYSTEM_TYPES_H
#define TYPES_SYSTEM_TYPES_H

#include "../utils/types.h"

#define GENERATE_ENUM(ENUM) ENUM,
#define GENERATE_STRING(STRING) #STRING,

#define FOREACH_TYPE(TYPE)  \
    TYPE(TYPE_VOID)         \
    TYPE(TYPE_BOOL)         \
                            \
    TYPE(TYPE_USIZE)        \
    TYPE(TYPE_SSIZE)        \
                            \
    TYPE(TYPE_UINT8)        \
    TYPE(TYPE_UINT16)       \
    TYPE(TYPE_UINT32)       \
    TYPE(TYPE_UINT64)       \
                            \
    TYPE(TYPE_INT8)         \
    TYPE(TYPE_INT16)        \
    TYPE(TYPE_INT32)        \
    TYPE(TYPE_INT64)        \
                            \
    TYPE(TYPE_F32)          \
    TYPE(TYPE_F64)          \
                            \
    TYPE(TYPE_POINTER)      \
    TYPE(TYPE_ARRAY)        \
                            \
    TYPE(TYPE_ENUM)         \
    TYPE(TYPE_STRUCT)       \
    TYPE(TYPE_FN_PTR)       \
                            \
    TYPE(TYPE_UNRESOLVED)

typedef enum {
    FOREACH_TYPE(GENERATE_ENUM)
} TypeKind;

static const char* TYPE_STRINGS[] = {
    FOREACH_TYPE(GENERATE_STRING)
};


typedef struct Type Type;
typedef struct Type {
    TypeKind kind;

    usize size;
    usize align;

    Type* pointer;
} Type;

#endif // !TYPES_SYSTEM_TYPES_H
