#include "hash.h"

u64 hash_fnv1a(const char* ptr, const usize len) {
    u64 hash = 2166136261u;

    for (usize i = 0; i < len; i++) {
        hash ^= ptr[i];
        hash *= 16777619u;
    }

    return hash;
} 
