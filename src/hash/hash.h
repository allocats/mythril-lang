#pragma once
#ifndef HASH_H
#define HASH_H

#include "../utils/types.h"

u64 hash_fnv1a(const char* p, const usize len);

#endif // !HASH_H
