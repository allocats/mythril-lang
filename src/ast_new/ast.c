#include "ast.h"
#include "types.h"
#include "parser/parser.h"

#include "../hash/hash.h"
#include "../utils/macros.h"

[[gnu::always_inline]]
b32 ast_is_decl(AstNode* node) {
    return node -> kind >= AST_IMPORT && node -> kind <= AST_STATIC_VAR;
}

[[gnu::always_inline]]
b32 ast_is_stmt(AstNode* node) {
    return node -> kind >= AST_VAR_DECL && node -> kind <= AST_RETURN;
}

[[gnu::always_inline]]
b32 ast_is_expr(AstNode* node) {
    return node -> kind >= AST_UNARY && node -> kind <= AST_IDENTIFIER;
}

void ast_error(Token* tok, const char* buf, const char* msg) {
    const char *p = buf;

    u32 line = 1;
    u32 col  = 1;

    while (p < tok -> literal) {
        if (*p == '\n') {
            line++;
            col = 1;
        } else {
            col++;
        }

        p++;
    }

    fprintf(stderr, "Error [%d:%d]: %s\n", line, col, msg);
    exit(1);
}

void ast_warn(Token* tok, const char* buf, const char* msg) {
    const char *p = buf;

    u32 line = 1;
    u32 col  = 1;

    while (p < tok -> literal) {
        if (*p == '\n') {
            line++;
            col = 1;
        } else {
            col++;
        }

        p++;
    }

    fprintf(stderr, "Warning [%d:%d]: %s\n", line, col, msg);
}

/*
*
*   Build the AST and then perform semantic analysis on it
*   From there build a HIR -> LIR. Need to figure out a link
*   between ast and symbols, I think hashes or node index
*
*/
Program* ast_build(ArenaAllocator* arena, Tokens* tokens, const char* buf) {
    Parser parser = {
        .arena = arena,
        .tokens = tokens -> items,
        .index = 0,
        .count = tokens -> count,
        .buffer = buf
    };

    Program* program = arena_alloc(arena, sizeof(*program));

    usize capacity = tokens -> count;

    MEOW_LOG("capacity=%zu", capacity);

    program -> nodes = arena_alloc(arena, sizeof(AstNode) * capacity);
    program -> count = 0;
    program -> capacity = capacity;

    while (parser.index < parser.count) {
        AstNode* node = &program -> nodes[program -> count++];
        Token token = *parser_peek(&parser); 
        
        switch (token.type) {
            case T_FUNCTION: {
                node = parse_function_decl(&parser);
            } break;

            case T_MODULE: {
                node = parse_module(&parser);
            } break;

            case T_IMPORT: {
                node = parse_import(&parser);
            } break;

            case T_STRUCT: {
                node = parse_struct_decl(&parser);
            } break;

            case T_ENUM: {
                node = parse_enum_decl(&parser);
            } break;

            case T_STATIC: {
                node = parse_static_decl(&parser);
            } break;

            case T_CONST: {
                node = parse_const_decl(&parser);
            } break;

            default: {
                ast_error(&token, buf, "Parsing bug, unknown token");
            } break;
        }
    }

    return program;
}

AstNode* ast_create_identifier(ArenaAllocator* arena, const char* ptr, const usize len) {
    AstNode* node = arena_alloc(arena, sizeof(*node));

    node -> kind = AST_IDENTIFIER;

    node -> identifier.ptr = ptr;
    node -> identifier.len = len;
    node -> identifier.hash = hash_fnv1a(ptr, len);

    return node;
}
