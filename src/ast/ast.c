#include "ast.h"

#include "../ast_parser/parser.h"
#include "../diagnostics/diagnostics.h"
#include "../hash/hash.h"
#include "../utils/vec.h"
#include "types.h"

#include <stdio.h>

void parse(MythrilContext* ctx, char** paths) {
    Tokens* tokens = ctx -> tokens;
    usize count = tokens -> count;

    Program* program = ctx -> program;

    program -> items = arena_alloc(ctx -> arena, sizeof(AstNode) * count);
    program -> capacity = count;

    Parser parser = {
        .arena = ctx -> arena,
        .tokens = tokens,
        .program = program,
        .index = 0,
        .count = count,
        .path = *paths++
    };

    while (parser.index < parser.count) {
        Token token = *parser_peek(&parser);

        #ifdef MYTHRIL_DEBUG
            printf("debug=%s\n", TOKEN_KIND_STRINGS[token.kind]);
        #endif

        AstNode* node = nullptr;
        extend_vec(program, ctx -> arena);

        switch (token.kind) {
            case TOK_MODULE: {
                node = parse_module_decl(ctx, &parser);
            } break;

            case TOK_IMPORT: {
                node = parse_import_decl(ctx, &parser);
            } break;

            case TOK_ENUM: {
                node = parse_enum_decl(ctx, &parser);
            } break;

            case TOK_STRUCT: {
                node = parse_struct_decl(ctx, &parser);
            } break;

            case TOK_IMPL: {
                node = parse_impl_decl(ctx, &parser);
            } break;

            case TOK_FUNCTION: {
                node = parse_function_decl(ctx, &parser);
            } break;

            case TOK_CONST: {
                node = parse_const_decl(ctx, &parser);
            } break;

            case TOK_STATIC: {
                node = parse_static_decl(ctx, &parser);
            } break;

            case TOK_RIGHT_BRACE: {
                parser_advance(&parser);
            } break;

            case TOK_EOF: {
                parser.path = *paths++;
                parser_advance(&parser);
            } break;

            case TOK_EOP: {
                return;
            } break;

            default: {
                SourceLocation location = source_location_from_token(
                    parser.path,
                    ctx -> buffer_start,
                    &token
                );

                diag_error(
                    ctx -> diag_ctx,
                    location,
                    "unexpected token '%.*s'",
                    token.length,
                    token.lexeme
                ); 
                return;
            } break;
        }

        if (node) {
            program -> items[program -> count++] = *node;
        }
    }
}

AstSlice* ast_make_slice_from_token(ArenaAllocator* arena, Token* token) {
    AstSlice* slice = arena_alloc(arena, sizeof(*slice));

    slice -> ptr = (char*) token -> lexeme; 
    slice -> len = token -> length; 
    slice -> hash = hash_fnv1a(token -> lexeme, token -> length); 

    return slice;
}
