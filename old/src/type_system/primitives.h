#pragma once
#ifndef PRIMITIVES_H
#define PRIMITIVES_H

#include "types.h"

static Type type_void = {
    .kind = TYPE_VOID,
    .size = 0,
    .align = 0
};

static Type type_bool = {
    .kind = TYPE_BOOL,
    .size = 1,
    .align = 1
};

static Type type_usize = {
    .kind = TYPE_USIZE,
    .size = 8,
    .align = 8
};

static Type type_ssize = {
    .kind = TYPE_SSIZE,
    .size = 8,
    .align = 8
};

static Type type_int8 = {
    .kind = TYPE_INT8,
    .size = 1,
    .align = 1
};

static Type type_int16 = {
    .kind = TYPE_INT16,
    .size = 2,
    .align = 2
};

static Type type_int32 = {
    .kind = TYPE_INT32,
    .size = 4,
    .align = 4
};

static Type type_int64 = {
    .kind = TYPE_INT64,
    .size = 8,
    .align = 8
};

static Type type_uint8 = {
    .kind = TYPE_UINT8,
    .size = 1,
    .align = 1
};

static Type type_uint16 = {
    .kind = TYPE_UINT16,
    .size = 2,
    .align = 2
};

static Type type_uint32 = {
    .kind = TYPE_UINT32,
    .size = 4,
    .align = 4
};

static Type type_uint64 = {
    .kind = TYPE_UINT64,
    .size = 8,
    .align = 8
};

static Type type_f32 = {
    .kind = TYPE_F32,
    .size = 4,
    .align = 4
};

static Type type_f64 = {
    .kind = TYPE_F64,
    .size = 8,
    .align = 8
};

#endif // !PRIMITIVES_H
