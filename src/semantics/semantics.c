#include "semantics.h"
#include "types.h"

#include "builtin_functions.h"

#include "../symbols/symbols.h"
#include "../type_system/primitives.h"
#include "../type_system/type_system.h"

#include <stdio.h>

SemanticCtx* create_sem_ctx(ArenaAllocator* arena) {
    SemanticCtx* ctx = arena_alloc(arena, sizeof(*ctx));

    ctx -> arena = arena;

    ctx -> symbols = create_symtable(arena);

    ctx -> types = arena_alloc(arena, sizeof(Type*) * 8);
    ctx -> type_capacity = 8;
    ctx -> type_count = 0;

    ctx -> current_node = nullptr;

    ctx -> error_count = 0;

    return ctx;
}

SemanticCtx* analyze_program(Program* program) {
    ArenaAllocator* arena = program -> arena;

    SemanticCtx* ctx = create_sem_ctx(arena);

    scope_enter(arena, ctx -> symbols);

    b32 found_main = false;

    // First pass: define all symbols
    for (usize i = 0; i < program -> count; i++) {
        AstNode* node = &program -> nodes[i];

        switch (node -> kind) {
            case AST_FUNCTION: {
                AstNode* name_node = node -> function.identifier;

                if (name_node -> identifier.hash == MAIN_HASH) {
                    found_main = true;
                }

                Symbol* sym = create_symbol(
                    arena,
                    SYMBOL_FUNCTION,
                    name_node -> identifier.ptr,
                    name_node -> identifier.len,
                    name_node -> identifier.hash
                );

                sym -> type = resolve_type(ctx, node -> function.return_type);
                sym -> node = node;

                define_symbol(arena, ctx -> symbols, sym);
            } break;

            default: {
                fprintf(stderr, "Whoops\n");
                exit(1);
            }
        }
    }

    if (!found_main) {
        // todo error
        fprintf(stderr, "Error: No main() function found\n");
        exit(1);
    }

    // Second pass: validate
    for (usize i = 0; i < program -> count; i++)  {
        AstNode* node = &program -> nodes[i];

        switch (node -> kind) {
            case AST_FUNCTION: {
                analyze_function(ctx, node);
            } break;

            default: {
                fprintf(stderr, "yikes\n");
                exit(1);
            }
        }
    }

    scope_exit(ctx -> symbols);

    return ctx;
}

void analyze_function(SemanticCtx* ctx, AstNode* fn) {
    ctx -> current_node = fn;

    scope_enter(ctx -> arena, ctx -> symbols);

    for (usize i = 0; i < fn -> function.params.count; i++) {
        AstNode* param = fn -> function.params.nodes[i];

        Symbol* param_sym = create_symbol(
            ctx -> arena,
            SYMBOL_VARIABLE,
            param -> var_decl.identifier -> identifier.ptr,
            param -> var_decl.identifier -> identifier.len,
            param -> var_decl.identifier -> identifier.hash
        );
        
        param_sym -> type = resolve_type(ctx, param -> var_decl.type);

        define_symbol(ctx -> arena, ctx -> symbols, param_sym); 
    }

    for (usize i = 0; i < fn -> function.block.count; i++) {
        analyze_statement(ctx, fn -> function.block.nodes[i]);
    }

    scope_exit(ctx -> symbols);

    ctx -> current_node = nullptr;
}

void analyze_statement(SemanticCtx* ctx, AstNode* stmt) {
    if (!stmt) {
        return;
    }

    switch (stmt -> kind) {
        default: {
            analyze_expr(ctx, stmt);
        }
    }
}

Type* analyze_expr(SemanticCtx* ctx, AstNode* expr) {
    if (!expr) {
        return &type_void;
    }

    switch (expr -> kind) {
        case AST_FN_CALL: {
            return analyze_fn_call(ctx, expr);
        } break;

        default: {
            return &type_void;
        }
    }
}

Type* analyze_fn_call(SemanticCtx* ctx, AstNode* node) {
    u64 hash = node -> fn_call.identifier -> identifier.hash;

    if (hash == SYSCALL_HASH) {
        if (node -> fn_call.args.count < 1 || node -> fn_call.args.count > 7) {
            // todo error
            fprintf(stderr, "Error: syscall() expects 1 to 7 arguments");
            exit(1);
        }

        for (usize i = 0; i < node -> fn_call.args.count; i++) {
            analyze_expr(ctx, node -> fn_call.args.nodes[i]);
        }

        return &type_int64;
    }

    Symbol* sym = symbol_lookup(ctx -> symbols, node -> fn_call.identifier -> identifier.hash);

    if (!sym || sym -> kind != SYMBOL_FUNCTION) {
        // todo error
        fprintf(stderr, "Error: Undefined function\n");
        exit(1);
    }

    for (usize i = 0; i < node -> fn_call.args.count; i++) {
        analyze_expr(ctx, node -> fn_call.args.nodes[i]);
    }

    return sym -> type;
}
