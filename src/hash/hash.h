#pragma once
#ifndef MYTHRIL_HASH_H
#define MYTHRIL_HASH_H

#include "../utils/types.h"

u64 hash_fnv1a(const char* ptr, const usize len);

#endif // !MYTHRIL_HASH_H
