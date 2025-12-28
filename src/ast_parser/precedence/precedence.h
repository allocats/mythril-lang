#pragma once
#ifndef MYTHRIL_AST_PARSER_PRECEDENCE_H
#define MYTHRIL_AST_PARSER_PRECEDENCE_H

#include "types.h"

#include "../../tokens/types.h"

u8  get_precedence(TokenKind kind); 
b32 is_right_associative(TokenKind kind); 
b32 is_binary_operator(TokenKind kind); 
b32 is_assignment_operator(TokenKind kind); 
b32 is_prefix_operator(TokenKind kind); 
b32 is_postfix_operator(TokenKind kind);

#endif // !MYTHRIL_AST_PARSER_PRECEDENCE_H
