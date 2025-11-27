#pragma once
#ifndef LEXER_H
#define LEXER_H

#include "../arena/arena.h"
#include "../token/token.h"

Tokens* lex_file(ArenaAllocator* arena, char* buffer, char* end);

#endif //!LEXER_H
