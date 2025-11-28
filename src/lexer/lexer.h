#pragma once
#ifndef MYTHRIL_LEXER_H
#define MYTHRIL_LEXER_H

#include "types.h"

#include "../mythril/types.h"

void tokenize(MythrilContext* ctx);

char* parse_word(MythrilContext* ctx, char* cursor);
char* parse_number(MythrilContext* ctx, char* cursor);
char* parse_operator(MythrilContext* ctx, char* cursor);
char* parse_delimiter(MythrilContext* ctx, char* cursor);
char* parse_string_literal(MythrilContext* ctx, char* cursor);
char* parse_invalid_tokens(MythrilContext* ctx, char* cursor);

#endif // !MYTHRIL_LEXER_H
