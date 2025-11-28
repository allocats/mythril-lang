#include "lexer.h"
#include "types.h"

#include "../diagnostics/diagnostics.h"
#include "../utils/types.h"
#include "../utils/vec.h"

#include <string.h>
#include <unistd.h>

#define SKIP_WHITESPACE(cursor) \
    while (IS_WHITESPACE(*cursor)) { \
        cursor++; \
    }

#define word_match(str, len, ptr) ((len) == sizeof((str)) - 1 && strncmp((str), (ptr), (len)) == 0) 

void tokenize(MythrilContext* ctx) {
    char* cursor = ctx -> buffer_start;
    char* end = ctx -> buffer_end;

    while (cursor < end) {
        SKIP_WHITESPACE(cursor);

        char c = *cursor;

        if (IS_ALPHA(c)) {
            cursor = parse_word(ctx, cursor);
        } else if (IS_DELIMITER(c)) {
            cursor = parse_delimiter(ctx, cursor);
        } else if (IS_OPERATOR(c)) {
            cursor = parse_operator(ctx, cursor);
        } else if (IS_DIGIT(c)) {
            cursor = parse_number(ctx, cursor);
        } else if (IS_STRING_DELIMS(c)) {
            cursor = parse_string_literal(ctx, cursor);
        } else {
            cursor = parse_invalid_tokens(ctx, cursor);
        }
    }
}

char* parse_word(MythrilContext* ctx, char* cursor) {
    Tokens* tokens = ctx -> tokens;
    ArenaAllocator* arena = ctx -> arena;

    extend_vec(tokens, arena);

    const char* start = cursor;

    while (IS_ALPHA(*cursor) || IS_DIGIT(*cursor)) {
        cursor++;
    }

    const usize len = cursor - start;

    Token* token = &tokens -> items[tokens -> count++];

    token -> lexeme = start;
    token -> length = len;

    switch (*start) {
        case 'b': {
            if (word_match("bool", len, start)) {
                token -> kind = TOK_BOOL;
                break;
            }

            token -> kind = TOK_IDENTIFIER;
        } break;

        case 'c': {
            if (word_match("char", len, start)) {
                token -> kind = TOK_CHAR;
                break;
            }

            if (word_match("const", len, start)) {
                token -> kind = TOK_CONST;
                break;
            }

            token -> kind = TOK_IDENTIFIER;
        } break;

        case 'e': {
            if (word_match("enum", len, start)) {
                token -> kind = TOK_ENUM;
                break;
            }

            token -> kind = TOK_IDENTIFIER;
        } break;

        case 'f': {
            if (word_match("fn", len, start)) {
                token -> kind = TOK_FUNCTION;
                break;
            }

            if (word_match("for", len, start)) {
                token -> kind = TOK_FOR;
                break;
            }

            if (word_match("f32", len, start)) {
                token -> kind = TOK_FLOAT_32;
                break;
            }

            if (word_match("f64", len, start)) {
                token -> kind = TOK_FLOAT_64;
                break;
            }

            if (word_match("false", len, start)) {
                token -> kind = TOK_FALSE;
                break;
            }

            token -> kind = TOK_IDENTIFIER;
        } break;

        case 'i': {
            if (word_match("i8", len, start)) {
                token -> kind = TOK_INT_8;
                break;
            }

            if (word_match("i16", len, start)) {
                token -> kind = TOK_INT_16;
                break;
            }

            if (word_match("i32", len, start)) {
                token -> kind = TOK_INT_32;
                break;
            }

            if (word_match("i64", len, start)) {
                token -> kind = TOK_INT_64;
                break;
            }

            if (word_match("if", len, start)) {
                token -> kind = TOK_IF;
                break;
            }

            if (word_match("in", len, start)) {
                token -> kind = TOK_IN;
                break;
            }

            if (word_match("impl", len, start)) {
                token -> kind = TOK_IMPL;
                break;
            }

            if (word_match("import", len, start)) {
                token -> kind = TOK_IMPORT;
                break;
            }

            token -> kind = TOK_IDENTIFIER;
        } break;

        case 'l': {
            if (word_match("let", len, start)) {
                token -> kind = TOK_LET;
                break;
            }

            token -> kind = TOK_IDENTIFIER;
        } break;

        case 'm': {
            if (word_match("match", len, start)) {
                token -> kind = TOK_MATCH;
                break;
            }

            token -> kind = TOK_IDENTIFIER;
        } break;

        case 'n': {
            if (word_match("null", len, start)) {
                token -> kind = TOK_NULL;
                break;
            }

            token -> kind = TOK_IDENTIFIER;
        } break;

        case 'r': {
            if (word_match("return", len, start)) {
                token -> kind = TOK_RETURN;
                break;
            }

            token -> kind = TOK_IDENTIFIER;
        } break;

        case 's': {
            if (word_match("self", len, start)) {
                token -> kind = TOK_SELF;
                break;
            }

            if (word_match("ssize", len, start)) {
                token -> kind = TOK_SSIZE;
                break;
            }

            if (word_match("struct", len, start)) {
                token -> kind = TOK_STRUCT;
                break;
            }

            if (word_match("static", len, start)) {
                token -> kind = TOK_STATIC;
                break;
            }

            token -> kind = TOK_IDENTIFIER;
        } break;

        case 't': {
            if (word_match("true", len, start)) {
                token -> kind = TOK_TRUE;
                break;
            }

            token -> kind = TOK_IDENTIFIER;
        } break;

        case 'u': {
            if (word_match("u8", len, start)) {
                token -> kind = TOK_UINT_8;
                break;
            }

            if (word_match("u16", len, start)) {
                token -> kind = TOK_UINT_16;
                break;
            }

            if (word_match("u32", len, start)) {
                token -> kind = TOK_UINT_32;
                break;
            }

            if (word_match("u64", len, start)) {
                token -> kind = TOK_UINT_64;
                break;
            }

            if (word_match("usize", len, start)) {
                token -> kind = TOK_USIZE;
                break;
            }

            if (word_match("uninit", len, start)) {
                token -> kind = TOK_UNINIT;
                break;
            }

            token -> kind = TOK_IDENTIFIER;
        } break;

        case 'v': {
            if (word_match("void", len, start)) {
                token -> kind = TOK_VOID;
                break;
            }

            token -> kind = TOK_IDENTIFIER;
        } break;

        case 'w': {
            if (word_match("while", len, start)) {
                token -> kind = TOK_WHILE;
                break;
            }

            token -> kind = TOK_IDENTIFIER;
        } break;

        default: {
            token -> kind = TOK_IDENTIFIER;
        } break;
    }

    return cursor;
}

char* parse_delimiter(MythrilContext* ctx, char* cursor) {
    Tokens* tokens = ctx -> tokens;
    ArenaAllocator* arena = ctx -> arena;

    extend_vec(tokens, arena);

    const char* start = cursor++;

    Token* token = &tokens -> items[tokens -> count++];

    token -> lexeme = start;
    token -> length = 1;

    switch (*start) {
        case '\0': {
            token -> kind = TOK_EOF;
        } break;

        case '(': {
            token -> kind = TOK_LEFT_PAREN;
        } break;

        case ')': {
            token -> kind = TOK_RIGHT_PAREN;
        } break;

        case '[': {
            token -> kind = TOK_LEFT_SQUARE;
        } break;

        case ']': {
            token -> kind = TOK_RIGHT_SQUARE;
        } break;

        case '{': {
            token -> kind = TOK_LEFT_BRACE;
        } break;

        case '}': {
            token -> kind = TOK_RIGHT_BRACE;
        } break;

        case ':': {
            token -> kind = TOK_COLON;
        } break;

        case ';': {
            token -> kind = TOK_SEMI_COLON;
        } break;

        case ',': {
            token -> kind = TOK_COMMA;
        } break;

        case '.': {
            token -> kind = TOK_DOT;
        } break;
    }

    return cursor;
}

char* parse_operator(MythrilContext* ctx, char* cursor) {
    Tokens* tokens = ctx -> tokens;
    ArenaAllocator* arena = ctx -> arena;

    extend_vec(tokens, arena);

    const char* start = cursor;

    while (IS_OPERATOR(*cursor)) {
        cursor++;
    }

    const char* end = cursor;
    const usize len = end - start;

    Token* token = &tokens -> items[tokens -> count++];
    token -> lexeme = start;
    token -> length = len;

    if (len == 2) {
        char c0 = start[0];
        char c1 = start[1];
        
        if (c1 == '=') {
            switch (c0) {
                case '!': token -> kind = TOK_BANG_EQUALS; return cursor;
                case '=': token -> kind = TOK_EQUALS_EQUALS; return cursor;
                case '/': token -> kind = TOK_SLASH_EQUALS; return cursor;
                case '+': token -> kind = TOK_PLUS_EQUALS; return cursor;
                case '-': token -> kind = TOK_MINUS_EQUALS; return cursor;
                case '*': token -> kind = TOK_STAR_EQUALS; return cursor;
                case '%': token -> kind = TOK_PERCENT_EQUALS; return cursor;
                case '<': token -> kind = TOK_LESS_THAN_EQUALS; return cursor;
                case '>': token -> kind = TOK_GREATER_THAN_EQUALS; return cursor;
                case '&': token -> kind = TOK_BIT_AND_EQUALS; return cursor;
                case '^': token -> kind = TOK_BIT_XOR_EQUALS; return cursor;
                case '~': token -> kind = TOK_BIT_NOT_EQUALS; return cursor;
                case '|': token -> kind = TOK_BIT_OR_EQUALS; return cursor;
            }
        }
        
        if (c0 == '+' && c1 == '+') { token -> kind = TOK_PLUS_PLUS; return cursor; }
        if (c0 == '-' && c1 == '-') { token -> kind = TOK_MINUS_MINUS; return cursor; }
        if (c0 == '<' && c1 == '<') { token -> kind = TOK_BIT_SHIFT_LEFT; return cursor; }
        if (c0 == '>' && c1 == '>') { token -> kind = TOK_BIT_SHIFT_RIGHT; return cursor; }
        if (c0 == '&' && c1 == '&') { token -> kind = TOK_COND_AND; return cursor; }
        if (c0 == '|' && c1 == '|') { token -> kind = TOK_COND_OR; return cursor; }
    }
    
    if (len == 1) {
        switch (*start) {
            case '!': token -> kind = TOK_BANG; return cursor;
            case '=': token -> kind = TOK_EQUALS; return cursor;
            case '%': token -> kind = TOK_PERCENT; return cursor;
            case '/': token -> kind = TOK_SLASH; return cursor;
            case '+': token -> kind = TOK_PLUS; return cursor;
            case '-': token -> kind = TOK_MINUS; return cursor;
            case '*': token -> kind = TOK_STAR; return cursor;
            case '<': token -> kind = TOK_LESS_THAN; return cursor;
            case '>': token -> kind = TOK_GREATER_THAN; return cursor;
            case '&': token -> kind = TOK_AMPERSAND; return cursor;
            case '^': token -> kind = TOK_BIT_XOR; return cursor;
            case '~': token -> kind = TOK_BIT_NOT; return cursor;
            case '|': token -> kind = TOK_BIT_OR; return cursor;
        }
    }
    
    if (len == 3) {
        if (start[0] == '<' && start[1] == '<' && start[2] == '=') {
            token -> kind = TOK_BIT_SHIFT_LEFT_EQUALS;
            return cursor;
        }
        if (start[0] == '>' && start[1] == '>' && start[2] == '=') {
            token -> kind = TOK_BIT_SHIFT_RIGHT_EQUALS;
            return cursor;
        }
    }

    token -> kind = TOK_ERROR;

    DiagContext* diag_ctx = ctx -> diag_ctx;
    SourceLocation location = source_location_from_token(diag_ctx -> path, ctx -> buffer_start, token);

    diag_error(diag_ctx, location, "unknown token found '%.*s'", token -> length, token -> lexeme); 

    return cursor;
}

char* parse_number(MythrilContext* ctx, char* cursor) {
    Tokens* tokens = ctx -> tokens;
    ArenaAllocator* arena = ctx -> arena;

    extend_vec(tokens, arena);

    const char* start = cursor;

    TokenKind kind = TOK_LITERAL_NUMBER;

    while (IS_DIGIT(*cursor)) {
        cursor++;
    }

    if (*cursor == '.') {
        if (IS_DIGIT(*(cursor + 1))) {
            kind = TOK_LITERAL_FLOAT;
            cursor++;

            while (IS_DIGIT(*cursor)) {
                cursor++;
            }
        }
    } 

    const char* end = cursor;
    const usize len = end - start;

    Token* token = &tokens -> items[tokens -> count++];

    token -> lexeme = start;
    token -> length = len;
    token -> kind = kind;

    return cursor;
}

char* parse_string_literal(MythrilContext* ctx, char* cursor) {
    Tokens* tokens = ctx -> tokens;
    ArenaAllocator* arena = ctx -> arena;

    extend_vec(tokens, arena);

    Token* opening_delimiter = &tokens -> items[tokens -> count++];

    opening_delimiter -> kind = TOK_STRING_DELIM;
    opening_delimiter -> lexeme = cursor;
    opening_delimiter -> length = 1;

    const char* start = ++cursor;

    while (!IS_STRING_DELIMS(*cursor)) {
        cursor++;

        if (*(cursor - 1) == '\\' && *cursor == '\"') {
            cursor++;
        }
    }

    const char* end = cursor;
    const usize len = end - start;

    Token* token = &tokens -> items[tokens -> count++];

    token -> kind = TOK_LITERAL_STRING;
    token -> lexeme = start;
    token -> length = len;

    Token* closing_delimiter = &tokens -> items[tokens -> count++];

    closing_delimiter -> kind = TOK_STRING_DELIM;
    closing_delimiter -> lexeme = cursor++;
    closing_delimiter -> length = 1;

    return cursor;
}

char* parse_invalid_tokens(MythrilContext* ctx, char* cursor) {
    Tokens* tokens = ctx -> tokens;
    ArenaAllocator* arena = ctx -> arena;

    extend_vec(tokens, arena);

    const char* start = ++cursor;

    while (
        !IS_WHITESPACE(*cursor) && 
        !IS_DELIMITER(*cursor) &&
        !IS_ALPHA(*cursor) &&
        !IS_DIGIT(*cursor) && 
        !IS_OPERATOR(*cursor) &&
        !IS_STRING_DELIMS(*cursor)
    ) {
        cursor++;
    }

    const char* end = cursor++;
    const usize len = end - start;

    Token* token = &tokens -> items[tokens -> count++];

    token -> kind = TOK_ERROR;
    token -> lexeme = start;
    token -> length = len;

    return cursor;
}
