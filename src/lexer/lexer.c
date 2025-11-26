#include "lexer.h"
#include "types.h"

#include "../utils/macros.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DEFAULT_CAP 256

#define SKIP_WHITESPACE(cursor) \
    while (IS_WHITESPACE(*cursor)) { \
        cursor++; \
    }

void error(
    const char* buffer,
    const char* cursor
) {
    const char* p = buffer;

    usize row = 1;
    usize col = 1;

    while (p != cursor) {
        if (*p == '\n') {
            row++;
            col = 1;
        } else {
            col++;
        }

        p++;
    }

    fprintf(stderr, "Found error %zu:%zu\n\n", row, col);
    // exit(1);
}

[[gnu::always_inline]]
void delimstack_push(
    DelimStack* stack,
    char value
) {
    // MEOW_LOG("Pushing %c onto delimiter stack", value);
    stack -> items[++stack -> top] = value;
}

[[gnu::always_inline]]
u8 delimstack_pop(
    DelimStack* stack
) {
    if (stack -> top < 0) return 0;

    u8 popped_delim = stack -> items[stack -> top--];
    return popped_delim;
}

char* parse_word(
    Tokens* tokens,
    ArenaAllocator* arena,
    char* cursor,
    char* end
) {
    char* start = cursor; 

    while (IS_ALPHA(*cursor) || IS_DIGIT(*cursor) || *cursor == '_') {
        cursor++;
    }

    if (cursor >= end) {
        return nullptr;
    }

    char placeback = *cursor; 
    *cursor = 0;

    if (MEOW_UNLIKELY(tokens -> count >= tokens -> capacity)) {
        TOKENS_REALLOC(tokens);
    } 

    usize len = cursor - start;

    Token* token = &(tokens -> items[tokens -> count++]);
    token -> literal = start;
    token -> len = len;

    switch (*start) {
        case 'b': {
            if (len == sizeof("bool") - 1 && strncmp(start, "bool", len) == 0) {
                token -> type = T_BOOL;
                break;
            }
            
            token -> type = T_IDENTIFIER;
        } break;

        case 'c': {
            if (len == sizeof("char") - 1 && strncmp(start, "char", len) == 0) {
                token -> type = T_CHAR;
                break;
            }

            if (len == sizeof("const") - 1 && strncmp(start, "const", len) == 0) {
                token -> type = T_CONST;
                break;
            }

            token -> type = T_IDENTIFIER;
        } break;

        case 'e': {
            if (len == sizeof("enum") - 1 && strncmp(start, "enum", len) == 0) {
                token -> type = T_ENUM;
                break;
            }

            token -> type = T_IDENTIFIER;
        } break;

        case 'f': {
            if (len == sizeof("fn") - 1 && strncmp(start, "fn", len) == 0) {
                token -> type = T_FUNCTION;
                break;
            }

            if (len == sizeof("f32") - 1 && strncmp(start, "f32", len) == 0) {
                token -> type = T_FLOAT32;
                break;
            }

            if (len == sizeof("f64") - 1 && strncmp(start, "f64", len) == 0) {
                token -> type = T_FLOAT64;
                break;
            }

            if (len == sizeof("for") - 1 && strncmp(start, "for", len) == 0) {
                token -> type = T_FOR;
                break;
            }

            token -> type = T_IDENTIFIER;
        } break;

        case 'i': {
            if (len == sizeof("if") - 1 && strncmp(start, "if", len) == 0) {
                token -> type = T_IF;
                break;
            }

            if (len == sizeof("i8") - 1 && strncmp(start, "i8", len) == 0) {
                token -> type = T_INT8;
                break;
            }

            if (len == sizeof("i16") - 1 && strncmp(start, "i16", len) == 0) {
                token -> type = T_INT16;
                break;
            }

            if (len == sizeof("i32") - 1 && strncmp(start, "i32", len) == 0) {
                token -> type = T_INT32;
                break;
            }

            if (len == sizeof("i64") - 1 && strncmp(start, "i64", len) == 0) {
                token -> type = T_INT64;
                break;
            }

            if (len == sizeof("import") - 1 && strncmp(start, "import", len) == 0) {
                token -> type = T_IMPORT;
                break;
            }

            if (len == sizeof("impl") - 1 && strncmp(start, "impl", len) == 0) {
                token -> type = T_IMPL;
                break;
            }

            token -> type = T_IDENTIFIER;
        } break;

        case 'l': {
            if (len == sizeof("let") - 1 && strncmp(start, "let", len) == 0) {
                token -> type = T_LET;
                break;
            }

            token -> type = T_IDENTIFIER;
        } break;

        case 'm': {
            if (len == sizeof("module") - 1 && strncmp(start, "module", len) == 0) {
                token -> type = T_MODULE;
                break;
            }

            token -> type = T_IDENTIFIER;
        } break;

        case 'n': {
            if (len == sizeof("null") - 1 && strncmp(start, "null", len) == 0) {
                token -> type = T_NULL;
                break;
            }

            if (len == sizeof("nullptr") - 1 && strncmp(start, "nullptr", len) == 0) {
                token -> type = T_NULLPTR;
                break;
            }

            token -> type = T_IDENTIFIER;
        } break;

        case 'u': {
            if (len == sizeof("u8") - 1 && strncmp(start, "u8", len) == 0) {
                token -> type = T_UINT8;
                break;
            }

            if (len == sizeof("u16") - 1 && strncmp(start, "u16", len) == 0) {
                token -> type = T_UINT16;
                break;
            }

            if (len == sizeof("u32") - 1 && strncmp(start, "u32", len) == 0) {
                token -> type = T_UINT32;
                break;
            }

            if (len == sizeof("u64") - 1 && strncmp(start, "u64", len) == 0) {
                token -> type = T_UINT64;
                break;
            }

            if (len == sizeof("usize") - 1 && strncmp(start, "usize", len) == 0) {
                token -> type = T_USIZE;
                break;
            }

            if (len == sizeof("uninit") - 1 && strncmp(start, "uninit", len) == 0) {
                token -> type = T_UNINIT;
                break;
            }

            token -> type = T_IDENTIFIER;
        } break;

        case 's': {
            if (len == sizeof("ssize") - 1 && strncmp(start, "ssize", len) == 0) {
                token -> type = T_SSIZE;
                break;
            }

            if (len == sizeof("struct") - 1 && strncmp(start, "struct", len) == 0) {
                token -> type = T_STRUCT;
                break;
            }

            if (len == sizeof("self") - 1 && strncmp(start, "self", len) == 0) {
                token -> type = T_SELF;
                break;
            }

            if (len == sizeof("static") - 1 && strncmp(start, "static", len) == 0) {
                token -> type = T_STATIC;
                break;
            }

            // if (len == sizeof("syscall") - 1 && strncmp(start, "syscall", len) == 0) {
            //     token -> type = T_SYSCALL;
            //     break;
            // }

            token -> type = T_IDENTIFIER;
        } break;

        case 'r': {
            if (len == sizeof("return") - 1 && strncmp(start, "return", len) == 0) {
                token -> type = T_RETURN;
                break;
            }

            token -> type = T_IDENTIFIER;
        } break;

        case 'v': {
            if (len == sizeof("void") - 1 && strncmp(start, "void", len) == 0) {
                token -> type = T_VOID;
                break;
            }

            token -> type = T_IDENTIFIER;
        } break;

        case 'w': {
            if (len == sizeof("while") - 1 && strncmp(start, "while", len) == 0) {
                token -> type = T_WHILE;
                break;
            }

            token -> type = T_IDENTIFIER;
        } break;

        default: {
            token -> type = T_IDENTIFIER;
        } break;
    }

    *cursor = placeback;

    return cursor;
}

char* parse_delim(
    Tokens* tokens,
    DelimStack* stack,
    ArenaAllocator* arena,
    const char* buffer,
    char* cursor,
    char* end
) {
    char* start = cursor; 

    if (MEOW_UNLIKELY(tokens -> count >= tokens -> capacity)) {
        TOKENS_REALLOC(tokens);
    } 

    Token* token = &(tokens -> items[tokens -> count++]);
    token -> literal = start;
    token -> len = 1;

    switch (*cursor) {
        case '\0': {
            token -> type = T_EOF;
            return cursor;
        } break;

        case ';': {
            // if (stack -> top != -1) {
            //     MEOW_ERROR("Missing closing delimiter");
            //     error(buffer, cursor);
            // }
            token -> type = T_SEMICOLON;
            return ++cursor;
        } break;

        case ':': {
            // if (stack -> top != -1) {
            //     MEOW_ERROR("Missing closing delimiter");
            //     error(buffer, cursor);
            // }
            token -> type = T_COLON;
            return ++cursor;
        } break;

        case ',': {
            token -> type = T_COMMA;
            return ++cursor;
        } break;

        case '.': {
            token -> type = T_DOT;
            return ++cursor;
        } break;

        case '(': {
            token -> type = T_LEFTPAREN;

            delimstack_push(stack, '(');
        } break;

        case ')': {
            token -> type = T_RIGHTPAREN;

            char opening_delimiter = delimstack_pop(stack);
            if (opening_delimiter != '(') {
                MEOW_ERROR("Missing opening delimiter '('");
                error(buffer, cursor);
                return nullptr;
            }
            // MEOW_ASSERT(opening_delimiter == '(', "Error! Expected opening delimiter '('");
        } break;

        case '[': {
            token -> type = T_LEFTSQUARE;

            delimstack_push(stack, '[');
        } break;

        case ']': {
            token -> type = T_RIGHTSQUARE;

            char opening_delimiter = delimstack_pop(stack);
            if (opening_delimiter != '[') {
                MEOW_ERROR("Missing opening delimiter '['");
                error(buffer, cursor);
                return nullptr;
            }
            // MEOW_ASSERT(opening_delimiter == '[', "Error! Expected opening delimiter '['");

        } break;

        case '{': {
            token -> type = T_LEFTBRACE;

            delimstack_push(stack, '{');
        } break;

        case '}': {
            token -> type = T_RIGHTBRACE;

            char opening_delimiter = delimstack_pop(stack);
            if (opening_delimiter != '{') {
                MEOW_ERROR("Missing opening delimiter '{'");
                error(buffer, cursor);
                return nullptr;
            }
            // MEOW_ASSERT(opening_delimiter == '{', "Error! Expected opening delimiter '{'");

        } break;
    }

    cursor++;

    return cursor;
}

char* parse_string(
    Tokens* tokens,
    ArenaAllocator* arena,
    char* cursor,
    char* end
) {
    char* start = cursor;
    char delimiter = *cursor++;

    if (MEOW_UNLIKELY(tokens -> count >= tokens -> capacity)) {
        TOKENS_REALLOC(tokens);
    } 

    Token* token = &(tokens -> items[tokens -> count++]);

    token -> type = T_STR_LITERAL;
    token -> literal = start;

    while (cursor < end) {
        if (*cursor == delimiter) {
            token -> len = ++cursor - start;
            return cursor;
        }

        cursor++;
    }

    MEOW_ERROR("Unterminated string");

    return nullptr;
}

char* parse_number(
    Tokens* tokens,
    ArenaAllocator* arena,
    char* cursor,
    char* end
) {
    char* start = cursor;

    if (MEOW_UNLIKELY(tokens -> count >= tokens -> capacity)) {
        TOKENS_REALLOC(tokens);
    } 

    Token* token = &(tokens -> items[tokens -> count++]);

    token -> type = T_NUMBER;
    token -> literal = start;

    while (cursor < end && IS_DIGIT(*cursor)) {
        cursor++;
    }

    if (IS_ALPHA(*cursor)) {
        MEOW_ERROR("Invalid token");
        return nullptr;
    }

    token -> len = cursor - start;

    return cursor;
}

char* parse_operator(
    Tokens* tokens,
    ArenaAllocator* arena,
    char* cursor,
    char* end
) {
    char* start = cursor; 

    if (MEOW_UNLIKELY(tokens -> count >= tokens -> capacity)) {
        TOKENS_REALLOC(tokens);
    } 

    Token* token = &(tokens -> items[tokens -> count++]);
    token -> literal = start;
    token -> len = 1;

    cursor++;

    if (IS_OPERATOR(*cursor)) {
        switch (*start) {
            case '&': {
                if (*cursor == '&') {
                    token -> type = T_COND_AND;
                    token -> len = 2;
                    return ++cursor;
                }
                
                if (*cursor == '=') {
                    token -> type = T_BIT_AND_EQUALS;
                    token -> len = 2;
                    return ++cursor;
                }
            } break;

            case '|': {
                if (*cursor == '|') {
                    token -> type = T_COND_OR;
                    token -> len = 2;
                    return ++cursor;
                }

                if (*cursor == '=') {
                    token -> type = T_BIT_OR_EQUALS;
                    token -> len = 2;
                    return ++cursor;
                }
            } break;

            case '~': {
                if (*cursor == '=') {
                    token -> type = T_BIT_NOT_EQUALS;
                    token -> len = 2;
                    return ++cursor;
                }
            } break;

            case '^': {
                if (*cursor == '=') {
                    token -> type = T_BIT_XOR_EQUALS;
                    token -> len = 2;
                    return ++cursor;
                }
            } break;

            case '!': {
                if (*cursor == '=') {
                    token -> type = T_BANGEQUALS;
                    token -> len = 2;
                    return ++cursor;
                }
            } break;

            case '=': {
                if (*cursor == '=') {
                    token -> type = T_EQUALSEQUALS;
                    token -> len = 2;
                    return ++cursor;
                }
            } break;

            case '>': {
                if (*cursor == '=') {
                    token -> type = T_LESSEQUALS;
                    token -> len = 2;
                    return ++cursor;
                }

                if (*cursor == '>') {
                    token -> type = T_BITSHIFT_RIGHT;
                    token -> len = 2;
                    return ++cursor;
                }
            } break;


            case '<': {
                if (*cursor == '=') {
                    token -> type = T_GREATEREQUALS;
                    token -> len = 2;
                    return ++cursor;
                }

                if (*cursor == '<') {
                    token -> type = T_BITSHIFT_LEFT;
                    token -> len = 2;
                    return ++cursor;
                }
            } break;

            case '+': {
                if (*cursor == '+') {
                    token -> type = T_PLUSPLUS;
                    token -> len = 2;
                    return ++cursor;
                }

                if (*cursor == '=') {
                    token -> type = T_PLUSEQUALS;
                    token -> len = 2;
                    return ++cursor;
                }
            } break;

            case '-': {
                if (*cursor == '-') {
                    token -> type = T_MINUSMINUS;
                    token -> len = 2;
                    return ++cursor;
                }

                if (*cursor == '=') {
                    token -> type = T_MINUSEQUALS;
                    token -> len = 2;
                    return ++cursor;
                }
            } break;

            case '*': {
                if (*cursor == '=') {
                    token -> type = T_ASTERIXEQUALS;
                    token -> len = 2;
                    return ++cursor;
                }
            } break;

            case '/': {
                if (*cursor == '=') {
                    token -> type = T_SLASHEQUALS;
                    token -> len = 2;
                    return ++cursor;
                }

                if (*cursor == '/') {
                    while (*cursor != '\n') cursor++;
                    tokens -> count--;
                    return ++cursor;
                }
            } break;
        }
    }

    switch (*start) {
        case '|': {
            token -> type = T_BIT_OR;
            token -> len = 1;
        } break;

        case '^': {
            token -> type = T_BIT_XOR;
            token -> len = 1;
        } break;

        case '~': {
            token -> type = T_BIT_NOT;
            token -> len = 1;
        } break;

        case '!': {
            token -> type = T_BANG;
            token -> len = 1;
        } break;

        case '=': {
            token -> type = T_EQUALS;
            token -> len = 1;
        } break;

        case '<': {
            token -> type = T_LESSTHAN;
            token -> len = 1;
        } break;

        case '>': {
            token -> type = T_GREATERTHAN;
            token -> len = 1;
        } break;

        case '+': {
            token -> type = T_PLUS;
            token -> len = 1;
        } break;

        case '-': {
            token -> type = T_MINUS;
            token -> len = 1;
        } break;

        case '*': {
            token -> type = T_ASTERIX;
            token -> len = 1;
        } break;

        case '/': {
            token -> type = T_SLASH;
            token -> len = 1;
        } break;

        case '&': {
            token -> type = T_AMPERSAND;
            token -> len = 1;
        } break;

        case '%': {
            token -> type = T_PERCENT;
            token -> len = 1;
        } break;
    }

    return cursor;
}

Tokens* lex_file(
    ArenaAllocator* arena,
    char* buffer,
    char* end
) {
    Tokens* tokens = arena_alloc(arena, sizeof(*tokens));

    tokens -> count = 0;
    tokens -> capacity = DEFAULT_CAP;
    tokens -> items = arena_alloc(arena, sizeof(Token) * DEFAULT_CAP);

    if (!tokens -> items) {
        MEOW_PANIC("Unable to allocate tokens -> items");
    }

    DelimStack delimiter_stack = {
        .items = {0},
        .top = -1
    };

    char* cursor = buffer;

    while (cursor < end) {
        SKIP_WHITESPACE(cursor);

        if (MEOW_UNLIKELY(cursor >= end)) return tokens;

        if (MEOW_LIKELY(IS_ALPHA(*cursor))) {
            cursor = parse_word(tokens, arena, cursor, end);
        } if (IS_DELIMITER(*cursor)) {
            cursor = parse_delim(tokens, &delimiter_stack, arena, buffer, cursor, end);
        } if (IS_STRING_DELIMS(*cursor)) {
            cursor = parse_string(tokens, arena, cursor, end);
        } if (IS_DIGIT(*cursor)) {
            cursor = parse_number(tokens, arena, cursor, end);
        } if (IS_OPERATOR(*cursor)) {
            cursor = parse_operator(tokens, arena, cursor, end);
        }

        if (cursor == nullptr) {
            error(buffer, cursor);
            return nullptr;
        }
    }

    return tokens;
}
