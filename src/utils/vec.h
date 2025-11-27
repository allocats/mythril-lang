#pragma once
#ifndef VEC_H
#define VEC_H

#include "../arena/arena.h"
#include "types.h"

#define push_vec(v, item, arena)                                                    \
    do {                                                                            \
        if ((v) -> count >= (v) -> capacity) {                                      \
            usize size = (v) -> count * sizeof(item);                               \
            (v) -> items = arena_realloc(arena, (v) -> items, size, size * 2);      \
            (v) -> capacity *= 2;                                                   \
        }                                                                           \
        (v) -> items[(v) -> count++] = item;                                        \
    } while(0);

#define extend_vec(v, arena)                                                        \
    do {                                                                            \
        if ((v) -> count >= (v) -> capacity) {                                      \
            usize size = (v) -> count * sizeof(*(v) -> items);                      \
            (v) -> items = arena_realloc(arena, (v) -> items, size, size * 2);      \
            (v) -> capacity *= 2;                                                   \
        }                                                                           \
    } while(0); 


#endif // !VEC_H
