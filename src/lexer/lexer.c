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

    const usize len = end - cursor;

    Token* token = &tokens -> items[tokens -> count++];

    token -> lexeme = start;
    token -> length = len;

    // todo: operators
}
