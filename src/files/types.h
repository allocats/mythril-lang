#pragma once
#ifndef MYTHRIL_FILES_TYPES_H
#define MYTHRIL_FILES_TYPES_H

#include "../utils/types.h"

typedef struct {
    char* ptr;
    usize len;
} FileBuffer;

#endif // !MYTHRIL_FILES_TYPES_H
