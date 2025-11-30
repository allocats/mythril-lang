#include "parser.h"

#include "../arena/arena.h"
#include "../ast/ast.h"
#include "../diagnostics/diagnostics.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

/* error handling */

void parser_recover_sync(Parser* p) {
    while (
        !parser_check(p, TOK_SEMI_COLON)    &&
        !parser_check(p, TOK_RIGHT_BRACE)   &&
        !parser_check(p, TOK_EOF) 
    ) {
        if (
            parser_check(p, TOK_IMPORT)     ||
            parser_check(p, TOK_STRUCT)     ||
            parser_check(p, TOK_ENUM)       ||
            parser_check(p, TOK_IMPL)       ||
            parser_check(p, TOK_FUNCTION)   ||
            parser_check(p, TOK_STATIC)     ||
            parser_check(p, TOK_CONST)
        ) {
            break;
        }

        parser_advance(p);
    }

    if (
        parser_check(p, TOK_SEMI_COLON)
    ) {
        parser_advance(p);
    }
}

void parser_error_at_current(MythrilContext* ctx, Parser* p, const char* msg, const char* help) {
    SourceLocation location = source_location_from_token(
        p -> path,
        ctx -> buffer_start,
        parser_peek(p)
    );

    diag_error_help(ctx -> diag_ctx, location, msg, help);
}

void parser_error_after_previous(MythrilContext* ctx, Parser* p, const char* msg, const char* help) {
    SourceLocation location = source_location_from_token(
        p -> path,
        ctx -> buffer_start,
        parser_previous(p)
    );

    location.column += location.length;
    location.pointer += location.length;
    location.length = 1;

    diag_error_help(ctx -> diag_ctx, location, msg, help);
}

void parser_error_at_previous(
    MythrilContext* ctx,
    Parser* p,
    const char* message,
    const char* help
) {
    SourceLocation location = source_location_from_token(
        p->path,
        ctx->buffer_start,
        parser_previous(p)
    );

    diag_error_help(ctx -> diag_ctx, location, message, help);
}

AstNode* parser_fail(Parser* p, AstNode* node) {
    parser_recover_sync(p);

    node -> kind = AST_ERROR;

    return node;
}

/* parsing */ 

#define MAX_PATH_SEGMENTS 64
#define SEGMENTS_SIZE (sizeof(AstSlice) * MAX_PATH_SEGMENTS)

bool parse_path_segments(MythrilContext* ctx, Parser* p, AstSlice* segments, usize* count) {
    if (!parser_require(ctx, p, TOK_IDENTIFIER, "module name before '::'")) {
        return false;
    }

    while (!parser_check(p, TOK_SEMI_COLON)) {
        if (*count > MAX_PATH_SEGMENTS) {
            parser_error_at_current(
                ctx,
                p,
                "too many path segments",
                "module paths are limited to 64 segments, sorry"
            );
        }

        Token* segment = parser_advance(p);
        segments[(*count)++] = *ast_make_slice_from_token(p -> arena, segment);

        if (parser_check(p, TOK_SEMI_COLON)) {
            break;
        }
        
        if (!parser_expect(ctx, p, TOK_COLON_COLON, "'::' or ';'")) {
            return false;
        }

        if (!parser_require(ctx, p, TOK_IDENTIFIER, "module name after '::'")) {
            return false;
        }
    }

    parser_advance(p);

    return true;
}

AstNode* parse_module_decl(MythrilContext* ctx, Parser* p) {
    AstNode* node = arena_alloc(p -> arena, sizeof(*node));

    node -> kind = AST_MODULE_DECL;
    node -> module_decl.count = 0;
    node -> module_decl.segments = arena_alloc(p -> arena, SEGMENTS_SIZE);

    parser_advance(p);

    if (!parse_path_segments(
        ctx,
        p,
        node -> module_decl.segments,
        &node -> module_decl.count
    )) {
        return parser_fail(p, node);
    }

    return node;
}

AstNode* parse_import_decl(MythrilContext* ctx, Parser* p) {
    AstNode* node = arena_alloc(p -> arena, sizeof(*node));

    node -> kind = AST_IMPORT_DECL;
    node -> import_decl.count = 0;
    node -> import_decl.segments = arena_alloc(p -> arena, SEGMENTS_SIZE); 

    parser_advance(p);

    if (!parse_path_segments(
        ctx,
        p,
        node -> import_decl.segments,
        &node -> import_decl.count
    )) {
        return parser_fail(p, node);
    }

    return node;
}

AstNode* parse_enum_decl(MythrilContext* ctx, Parser* p) {
    AstNode* node = arena_alloc(p -> arena, sizeof(*node));

    return node;
}

AstNode* parse_struct_decl(MythrilContext* ctx, Parser* p) {
    AstNode* node = arena_alloc(p -> arena, sizeof(*node));

    return node;
}

AstNode* parse_impl_decl(MythrilContext* ctx, Parser* p) {
    AstNode* node = arena_alloc(p -> arena, sizeof(*node));

    return node;
}

AstNode* parse_function_decl(MythrilContext* ctx, Parser* p) {
    AstNode* node = arena_alloc(p -> arena, sizeof(*node));

    node -> kind = AST_FUNCTION_DECL;
    node -> function_decl.count = 0;
    node -> function_decl.capacity = 64;
    node -> function_decl.parameters = arena_alloc(p -> arena, sizeof(AstParameter) * 64);

    parser_advance(p);

    if (!parser_require(ctx, p, TOK_IDENTIFIER, "function name")) {
        return parser_fail(p, node);
    }

    Token* name = parser_advance(p);
    node -> function_decl.identifier = *ast_make_slice_from_token(p -> arena, name);

    if (!parser_expect(ctx, p, TOK_LEFT_PAREN, "'('")) {
        return parser_fail(p, node);
    }

    while (!parser_check(p, TOK_RIGHT_PAREN)) {
        if (node -> function_decl.count >= node -> function_decl.capacity) {
            usize old_size = node -> function_decl.capacity * sizeof(AstParameter);
            usize new_size = old_size * 2;

            node -> function_decl.parameters = arena_realloc(
                p -> arena,
                node -> function_decl.parameters,
                old_size,
                new_size
            );

            node -> function_decl.capacity *= 2;
        }

        if (!parser_require(ctx, p, TOK_IDENTIFIER, "parameter name")) {
            return parser_fail(p, node);
        }

        Token* param_name = parser_advance(p);

        if (!parser_expect(ctx, p, TOK_COLON, "':' between parameter name and type")) {
            return parser_fail(p, node);
        }

        usize index = node -> function_decl.count;
        AstParameter* param = &node -> function_decl.parameters[index];

        param -> identifier = *ast_make_slice_from_token(p -> arena, param_name);
        param -> type = parse_type(ctx, p);

        if (param -> type == nullptr) {
            return parser_fail(p, node);
        }

        if (parser_check(p, TOK_RIGHT_PAREN)) {
            break;
        }

        if (!parser_expect(ctx, p, TOK_COMMA, "',' between parameters")) {
            return parser_fail(p, node);
        }
    }

    if (!parser_expect(ctx, p, TOK_RIGHT_PAREN, "')'")) {
        return parser_fail(p, node);
    }
    
    if (!parser_expect(ctx, p, TOK_COLON, "':' before return type")) {
        return parser_fail(p, node);
    }
    
    node -> function_decl.return_type = parse_type(ctx, p);

    if (node -> function_decl.return_type == NULL) {
        return parser_fail(p, node);
    }
    
    if (!parser_expect(ctx, p, TOK_LEFT_BRACE, "'{'")) {
        return parser_fail(p, node);
    }
    
    node -> function_decl.block = parse_block(ctx, p);
    
    if (!parser_expect(ctx, p, TOK_RIGHT_BRACE, "'}'")) {
        return parser_fail(p, node);
    }

    return node;
}

AstNode* parse_const_decl(MythrilContext* ctx, Parser* p) {
    AstNode* node = arena_alloc(p -> arena, sizeof(*node));

    return node;
}

AstNode* parse_static_decl(MythrilContext* ctx, Parser* p) {
    AstNode* node = arena_alloc(p -> arena, sizeof(*node));

    return node;
}

AstNode* parse_var_decl(MythrilContext* ctx, Parser *p) {
    AstNode* node = arena_alloc(p -> arena, sizeof(*node));

    node -> kind = AST_VAR_DECL;

    parser_advance(p);

    if (!parser_require(ctx, p, TOK_IDENTIFIER, "variable name")) {
        return parser_fail(p, node);
    }

    Token* name = parser_advance(p);

    if (!parser_expect(ctx, p, TOK_COLON, "':' between identifer and type")) {
        return parser_fail(p, node);
    }

    node -> var_decl.identifier = *ast_make_slice_from_token(p -> arena, name);
    node -> var_decl.type = parse_type(ctx, p);
    node -> var_decl.value = nullptr;

    if (node -> var_decl.type == nullptr) {
        return parser_fail(p, node);
    }

    if (parser_check(p, TOK_SEMI_COLON)) {
        parser_advance(p);
        return node;
    }

    if (!parser_expect(ctx, p, TOK_EQUALS, "'=' or ';'")) {
        return parser_fail(p, node);
    }

    node -> var_decl.value = parse_expression(ctx, p);

    return node;
}

AstVec parse_block(MythrilContext* ctx, Parser* p) {
    AstVec vec = {
        .items = arena_alloc(p -> arena, sizeof(AstNode*) * 8),
        .count = 0,
        .capacity = 8
    };

    while (!parser_check(p, TOK_RIGHT_BRACE)) {
        AstNode* statement = parse_statement(ctx, p);

        if (vec.count >= vec.capacity) {
            usize size = vec.count * sizeof(*vec.items);

            vec.items = arena_realloc(p -> arena, vec.items, size, size * 2);
            vec.capacity *= 2;
        }

        vec.items[vec.count++] = statement;
    }

    if (vec.count == 0) {
        // diag_warning(ctx -> diag_ctx, SourceLocation loc, const char *fmt, ...)
    }

    return vec;
}

AstNode* parse_statement(MythrilContext* ctx, Parser* p) {
    AstNode* node = nullptr;

    switch (parser_peek(p) -> kind) {
        case TOK_LET: {
            node = parse_var_decl(ctx, p);
        } break;

        case TOK_CONST: {
            node = parse_const_decl(ctx, p);
        } break;

        case TOK_IDENTIFIER: {
            node = parse_expression(ctx, p);
        } break;

        case TOK_LOOP: {
            node = parse_loop_stmt(ctx, p);
        } break;

        case TOK_RETURN: {
            node = parse_return_stmt(ctx, p);
        } break;

        default: {
        } break;
    }

    return node;
}


AstNode* parse_loop_stmt(MythrilContext* ctx, Parser* p) {
    AstNode* node = arena_alloc(p -> arena, sizeof(*node));

    return node;
}

AstNode* parse_expression(MythrilContext* ctx, Parser* p) {
    AstNode* node = arena_alloc(p -> arena, sizeof(*node));

    return node;
}

AstNode* parse_return_stmt(MythrilContext* ctx, Parser* p) {
    AstNode* node = arena_alloc(p -> arena, sizeof(*node));

    return node;
}

AstType* parse_type(MythrilContext* ctx, Parser* p) {
    if (!parser_require(ctx, p, TOK_IDENTIFIER, "type identifier")) {
        parser_recover_sync(p);
        return NULL;
    }
    
    Token* base_token = parser_advance(p);

    AstType* base_type = arena_alloc(p -> arena, sizeof(*base_type));
    base_type -> kind = TYPE_BASIC;
    base_type -> identifier = *ast_make_slice_from_token(p -> arena, base_token);
    
    AstType* result = base_type;
    
    while (true) {
        if (parser_check(p, TOK_STAR)) {
            parser_advance(p);
            
            AstType* ptr_type = arena_alloc(p -> arena, sizeof(*ptr_type));

            ptr_type -> kind = TYPE_POINTER;
            ptr_type -> pointee = result;

            result = ptr_type;
            
        } else if (parser_check(p, TOK_LEFT_SQUARE)) {
            parser_advance(p);
            
            AstType* array_type = arena_alloc(p -> arena, sizeof(*array_type));

            array_type -> kind = TYPE_ARRAY;
            array_type -> array.element_type = result;
            
            if (!parser_check(p, TOK_RIGHT_SQUARE)) {
                array_type -> array.size_expr = parse_expression(ctx, p);
                
                if (array_type -> array.size_expr == NULL) {
                    parser_error_at_current(
                        ctx,
                        p,
                        "expected array size expression or ']'",
                        "provide constant expression like 10 or 'BUFFER_SIZE'" 
                    );
                    parser_recover_sync(p);
                    return NULL;
                }
            } else {
                array_type -> array.size_expr = NULL;
            }
            
            if (!parser_expect(ctx, p, TOK_RIGHT_SQUARE, "']'")) {
                parser_recover_sync(p);

                return NULL;
            }

            parser_advance(p);
            
            result = array_type;
            
        } else {
            break;
        }
    }
    
    return result;
}


u64 parse_integer(char* ptr, usize len) {
    char buffer[32];

    memcpy(buffer, ptr, len);
    buffer[len] = 0;

    return strtoull(buffer, NULL, 10);
}

bool parser_expect(MythrilContext* ctx, Parser* p, TokenKind kind, const char* what) {
    if (!parser_check(p, kind)) {
        char msg[128];
        char help[128];

        snprintf(msg, sizeof(msg), "expected %s", what);
        snprintf(help, sizeof(help), "add %s here", what);

        parser_error_after_previous(ctx, p, msg, help);

        return false;
    }

    parser_advance(p);

    return true;
}

bool parser_require(MythrilContext* ctx, Parser* p, TokenKind kind, const char* what) {
    if (!parser_check(p, kind)) {
        char msg[128];
        char help[128];

        snprintf(msg, sizeof(msg), "expected %s", what);
        snprintf(help, sizeof(help), "add %s", what);

        parser_error_at_current(ctx, p, msg, help);

        return false;
    }

    return true;
}

inline Token* parser_previous(Parser* p) {
    if (p -> index <= 0) {
        fprintf(stderr, "error: tokens underflow OOB\n");
        exit(1);
    }

    return &p -> tokens -> items[p -> index - 1];
}

inline Token* parser_peek(Parser* p) {
    if (p -> index >= p -> count) {
        fprintf(stderr, "error: sudden end of tokens OOB\n");
        exit(1);
    }

    return &p -> tokens -> items[p -> index];
}

inline Token* parser_peek_ahead(Parser* p) {
    if (p -> index >= p -> count) {
        fprintf(stderr, "error: sudden end of tokens OOB\n");
        exit(1);
    }

    return &p -> tokens -> items[p -> index + 1];
}

inline Token* parser_advance(Parser* p) {
    if (p -> index >= p -> count) {
        fprintf(stderr, "error: sudden end of tokens OOB\n");
        exit(1);
    }

    return &p -> tokens -> items[p -> index++];
}

inline bool parser_check(Parser* p, TokenKind kind) {
    Token* tok = parser_peek(p);
    return tok -> kind == kind;
}

inline bool parser_check_ahead(Parser* p, TokenKind kind) {
    Token* tok = parser_peek_ahead(p);
    return tok -> kind == kind;
}
