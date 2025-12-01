#pragma once
#ifndef MYTHRIL_AST_PARSER_PRECEDENCE_TYPES_H
#define MYTHRIL_AST_PARSER_PRECEDENCE_TYPES_H

#include "../../tokens/types.h"

#define UNARY_PRECEDENCE 12

static const u8 PRECEDENCE_MAP[256] = {
    [TOK_STAR]                  = 11,
    [TOK_SLASH]                 = 11,
    [TOK_PERCENT]               = 11,
    
    [TOK_PLUS]                  = 10,
    [TOK_MINUS]                 = 10,
    
    [TOK_BIT_SHIFT_LEFT]        = 9,
    [TOK_BIT_SHIFT_RIGHT]       = 9,
    
    [TOK_LESS_THAN]             = 8,
    [TOK_LESS_THAN_EQUALS]      = 8,
    [TOK_GREATER_THAN]          = 8,
    [TOK_GREATER_THAN_EQUALS]   = 8,
    
    [TOK_EQUALS_EQUALS]         = 7,
    [TOK_BANG_EQUALS]           = 7,
    
    [TOK_AMPERSAND]             = 6,
    
    [TOK_BIT_XOR]               = 5,
    
    [TOK_BIT_OR]                = 4,
    
    [TOK_COND_AND]              = 3,
    
    [TOK_COND_OR]               = 2,
    
    [TOK_EQUALS]                = 1,
    [TOK_PLUS_EQUALS]           = 1,
    [TOK_MINUS_EQUALS]          = 1,
    [TOK_STAR_EQUALS]           = 1,
    [TOK_SLASH_EQUALS]          = 1,
    [TOK_BIT_AND_EQUALS]        = 1,
    [TOK_BIT_OR_EQUALS]         = 1,
    [TOK_BIT_XOR_EQUALS]        = 1,
    [TOK_BIT_NOT_EQUALS]        = 1,
};

static const b32 RIGHT_ASSOC_MAP[256] = {
    [TOK_EQUALS]            = true,
    [TOK_PLUS_EQUALS]       = true,
    [TOK_MINUS_EQUALS]      = true,
    [TOK_STAR_EQUALS]       = true,
    [TOK_SLASH_EQUALS]      = true,
    [TOK_BIT_AND_EQUALS]    = true,
    [TOK_BIT_OR_EQUALS]     = true,
    [TOK_BIT_XOR_EQUALS]    = true,
    [TOK_BIT_NOT_EQUALS]    = true,
};


#endif // !MYTHRIL_AST_PARSER_PRECEDENCE_TYPES_H
