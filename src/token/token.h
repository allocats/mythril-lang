#pragma once
#ifndef TOKEN_H
#define TOKEN_H

#include "types.h"

#include <assert.h>


#define TOKENS_REALLOC(tokens) \
    do { \
        usize new_cap = (tokens) -> capacity * 2;\
        (tokens) -> items = arena_realloc(arena, (tokens) -> items, (tokens) -> capacity, new_cap); \
        (tokens) -> capacity = new_cap; \
    } while(0);

#define IS_PRIMITIVE_TYPE(n) ((n) >= T_INT8 && (n) <= T_SSIZE)

void tokens_print(Tokens* tokens);

#endif // !TOKEN_H
