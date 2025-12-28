#include "ast.h"

#include "../ast_parser/parser.h"
#include "../diagnostics/diagnostics.h"
#include "../hash/hash.h"
#include "types.h"

static void extend_declarations(ArenaAllocator* arena, Program* prog);

void parse(MythrilContext* ctx, char** paths, FileBuffer* buffers, usize file_count) {
    Tokens* tokens = ctx -> tokens;
    usize count = tokens -> count;

    Program* program = ctx -> program;

    program -> declarations = arena_alloc(ctx -> arena, sizeof(AstNode*) * count);
    program -> capacity = count;

    Parser parser = {
        .arena = ctx -> arena,
        .tokens = tokens,
        .program = program,
        .index = 0,
        .count = count,
        .path = *paths++,
        .delimiters = {0}
    };

    u32 buffer_idx = 1;

    while (parser.index < parser.count) {
        Token* token = parser_peek(&parser);

        AstNode* node = nullptr;
        
        extend_declarations(ctx -> arena, program);

        switch (token -> kind) {
            case TOK_MODULE: {
                parser_advance(&parser);
                node = parse_module_decl(ctx, &parser);
            } break;

            case TOK_IMPORT: {
                parser_advance(&parser);
                node = parse_import_decl(ctx, &parser);
            } break;

            case TOK_ENUM: {
                parser_advance(&parser);
                node = parse_enum_decl(ctx, &parser);
            } break;

            case TOK_STRUCT: {
                parser_advance(&parser);
                node = parse_struct_decl(ctx, &parser);
            } break;

            case TOK_IMPL: {
                parser_advance(&parser);
                node = parse_impl_decl(ctx, &parser);
            } break;

            case TOK_FUNCTION: {
                parser_advance(&parser);
                node = parse_function_decl(ctx, &parser);
            } break;

            case TOK_CONST: {
                parser_advance(&parser);
                node = parse_const_decl(ctx, &parser);
            } break;

            case TOK_STATIC: {
                parser_advance(&parser);
                node = parse_static_decl(ctx, &parser);
            } break;

            case TOK_EOF: {
                if (buffer_idx < file_count) {
                    parser.path = *paths++;

                    ctx -> buffer_start = buffers[buffer_idx].ptr;
                    ctx -> buffer_end = buffers[buffer_idx].ptr + buffers[buffer_idx].len; 

                    buffer_idx++;
                }

                parser_advance(&parser);
            } break;

            case TOK_EOP: {
                return;
            } break;

            default: {
                SourceLocation location = source_location_from_token(
                    parser.path,
                    ctx -> buffer_start,
                    token
                );

                diag_error(
                    ctx -> diag_ctx,
                    location,
                    "unexpected top level declaration '%.*s'",
                    token -> length,
                    token -> lexeme
                ); 

                recover_to_top_level_decl(&parser);
            } break;
        }

        if (node) {
            program -> declarations[program -> count++] = node;
        }
    }
}

AstSlice* make_slice_from_token(ArenaAllocator* arena, Token* token) {
    AstSlice* slice = arena_alloc(arena, sizeof(*slice));

    slice -> ptr  = token -> lexeme; 
    slice -> len  = token -> length; 
    slice -> hash = hash_fnv1a(token -> lexeme, token -> length); 

    return slice;
}

static void extend_declarations(ArenaAllocator* arena, Program* prog) {
    if (prog -> count < prog -> capacity) {
        return;
    }

    usize size = prog -> capacity * sizeof(AstNode*); 

    prog -> declarations = arena_realloc(arena, prog -> declarations, size, size * 2);
    prog -> capacity *= 2;
}
