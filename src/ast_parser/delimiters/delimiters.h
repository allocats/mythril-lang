#pragma once
#ifndef MYTHRIL_AST_PARSER_DELIMITERS_H
#define MYTHRIL_AST_PARSER_DELIMITERS_H

#include "../parser.h"

void delimiters_push(Parser* p, Token* token, const char* src_ctx, const usize length);
Delimiter* delimiters_pop(Parser* p);

#endif // !MYTHRIL_AST_PARSER_DELIMITERS_H
