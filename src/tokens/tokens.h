#pragma once
#ifndef MYTHRIL_TOKENS_H
#define MYTHRIL_TOKENS_H

#include "types.h"

#define IS_PRIMITIVE_TYPE(x) ((x) >= TOK_UINT_8 && (x) <= TOK_BOOL)

void print_tokens(Tokens tokens);

#endif // !MYTHRIL_TOKENS_H
