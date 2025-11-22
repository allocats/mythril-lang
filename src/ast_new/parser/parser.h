#pragma once
#ifndef AST_PARSER_H
#define AST_PARSER_H

#include "types.h"

Token* parser_peek(Parser* p);
Token* parser_advance(Parser* p);

#endif // !AST_PARSER_H
