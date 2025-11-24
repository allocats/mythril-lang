#pragma once
#ifndef LEXER_TYPES_H
#define LEXER_TYPES_H

#include "../utils/types.h"

#define IS_DIGIT(c) (char_map[(unsigned char)(c)] & 1)
#define IS_ALPHA(c) (char_map[(unsigned char)(c)] & 2)
#define IS_OPERATOR(c) (char_map[(unsigned char)(c)] & 4)
#define IS_DELIMITER(c) (char_map[(unsigned char)(c)] & 8)
#define IS_WHITESPACE(c) (char_map[(unsigned char)(c)] & 16)
#define IS_STRING_DELIMS(c) (char_map[(unsigned char)(c)] & 32)

#define DELIM_STACK_MAX 256 

static const u8 char_map[256] = {
    ['0' ... '9'] = 1,

    ['a' ... 'z'] = 2,
    ['A' ... 'Z'] = 2,
    
    ['&'] = 4,
    ['|'] = 4,
    ['~'] = 4,
    ['^'] = 4,
    ['-'] = 4,
    ['+'] = 4,
    ['/'] = 4,
    ['*'] = 4,
    ['%'] = 4,
    ['='] = 4,
    ['!'] = 4,
    ['<'] = 4,
    ['>'] = 4,

    ['.'] = 8,
    [','] = 8,
    ['['] = 8,
    [']'] = 8,
    ['('] = 8,
    [')'] = 8,
    ['{'] = 8,
    ['}'] = 8,
    [';'] = 8,
    [':'] = 8,

    [' '] = 16, 
    ['\t'] = 16, 
    ['\n'] = 16,
    ['\f'] = 16,
    ['\r'] = 16,

    ['\''] = 32,
    ['\"'] = 32,
};

typedef struct {
    u8  items[DELIM_STACK_MAX];
    i32 top;
} DelimStack;

#endif // !LEXER_TYPES_H
