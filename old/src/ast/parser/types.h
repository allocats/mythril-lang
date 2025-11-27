#pragma once
#ifndef AST_PARSER_TYPES_H
#define AST_PARSER_TYPES_H

#include "../../arena/arena.h"
#include "../../token/types.h"
#include "../../utils/types.h"

typedef struct {
    ArenaAllocator* arena;
    Token* tokens;
    usize count;
    usize index;

    const char* buffer; // Buffer for error handling
    b32 had_error;
} Parser;

#define UNARY_PRECEDENCE 12

static const u8 PRECEDENCE_MAP[TOKEN_TYPE_COUNT] = {
    [T_ASTERIX]        = 11,
    [T_SLASH]          = 11,
    [T_PERCENT]        = 11,
    
    [T_PLUS]           = 10,
    [T_MINUS]          = 10,
    
    [T_BITSHIFT_LEFT]  = 9,
    [T_BITSHIFT_RIGHT] = 9,
    
    [T_LESSTHAN]       = 8,
    [T_LESSEQUALS]     = 8,
    [T_GREATERTHAN]    = 8,
    [T_GREATEREQUALS]  = 8,
    
    [T_EQUALSEQUALS]   = 7,
    [T_BANGEQUALS]     = 7,
    
    [T_AMPERSAND]      = 6,
    
    [T_BIT_XOR]        = 5,
    
    [T_BIT_OR]         = 4,
    
    [T_COND_AND]       = 3,
    
    [T_COND_OR]        = 2,
    
    [T_EQUALS]         = 1,
    [T_PLUSEQUALS]     = 1,
    [T_MINUSEQUALS]    = 1,
    [T_ASTERIXEQUALS]  = 1,
    [T_SLASHEQUALS]    = 1,
    [T_BIT_AND_EQUALS] = 1,
    [T_BIT_OR_EQUALS]  = 1,
    [T_BIT_XOR_EQUALS] = 1,
    [T_BIT_NOT_EQUALS] = 1,
};

static const b32 RIGHT_ASSOC_MAP[TOKEN_TYPE_COUNT] = {
    [T_EQUALS]         = true,
    [T_PLUSEQUALS]     = true,
    [T_MINUSEQUALS]    = true,
    [T_ASTERIXEQUALS]  = true,
    [T_SLASHEQUALS]    = true,
    [T_BIT_AND_EQUALS] = true,
    [T_BIT_OR_EQUALS]  = true,
    [T_BIT_XOR_EQUALS] = true,
    [T_BIT_NOT_EQUALS] = true,
};

#endif // !AST_PARSER_TYPES_H
