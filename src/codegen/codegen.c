#include "codegen.h"
#include "types.h"

#include "../semantics/builtin_functions.h"
#include "../symbols/symbols.h"

#include <stdio.h>
#include <string.h>

static const char* arg_registers[] = {
    "rdi",
    "rsi",
    "rdx",
    "rcx",
    "r8",
    "r9"
};

i64 calculate_stack_size(SemanticCtx* ctx, AstNode* fn) {
    i64 size = 0;
    
    for (usize i = 0; i < fn -> function.block.count; i++) {
        AstNode* stmt = fn -> function.block.nodes[i];

        if (stmt -> kind == AST_VAR_DECL) {
            Symbol* sym = symbol_lookup(
                ctx -> symbols,
                stmt -> var_decl.identifier -> identifier.hash
            );

            size += sym -> type -> size;
        }
    }
    
    return (size + 15) & ~15;
}

void codegen(Program *prog, SemanticCtx *sem) {
    FILE* out = fopen("output.asm", "w");

    Codegen cg_ctx = {
        .arena = prog -> arena,
        .output = out,
        .current_node = nullptr,
        .sem_ctx = sem,
        .strings = arena_alloc(prog -> arena, sizeof(StringLiteral) * 16),
        .string_count = 0,
        .string_cap = 16
    };

    fprintf(out, "section .text\n");
    fprintf(out, "global _start\n\n");

    for (usize i = 0; i < prog -> count; i++) {
        AstNode* node = &prog -> nodes[i];

        switch (node -> kind) {
            case AST_FUNCTION: {
                gen_function(&cg_ctx, node);
            } break;

            default: {
                printf("BUSY");
                exit(1);
            }
        }
    }

    fprintf(out, "_start:\n");
    fprintf(out, "    call main\n");
    fprintf(out, "    mov rax, 60\n");
    fprintf(out, "    mov rdi, 0\n");
    fprintf(out, "    syscall\n\n");

    gen_data_section(&cg_ctx);

    fclose(out);
}

void gen_function(Codegen* ctx, AstNode* node) {
    ctx -> current_node = node;

    FILE* out = ctx -> output;

    fprintf(
        out,
        "%.*s:\n",
        (i32) node -> function.identifier -> identifier.len,
        node -> function.identifier -> identifier.ptr
    );

    fprintf(out, "    push rbp\n");
    fprintf(out, "    mov rbp, rsp\n");

    i64 stack_size = calculate_stack_size(ctx -> sem_ctx, node);

    if (stack_size > 0) {
        fprintf(out, "    sub rsp, %ld\n", stack_size);
    }

    fprintf(out, "\n");
    
    for (usize i = 0; i < node -> function.params.count && i < 6; i++) {
        AstNode* param = node -> function.params.nodes[i];
        Symbol* sym = symbol_lookup(
            ctx -> sem_ctx -> symbols,
            param -> var_decl.identifier -> identifier.hash
        );

        ctx -> stack_offset -= 8;
        sym -> stack_offset = ctx -> stack_offset;

        fprintf(out, "    mov [rbp%ld], %s\n", sym -> stack_offset, arg_registers[i]);
    }

    for (usize i = 0; i < node -> function.block.count; i++) {
        gen_statement(ctx, node -> function.block.nodes[i]);
    }

    fprintf(
        out,
        ".end_%.*s:\n",
        (i32) node -> function.identifier -> identifier.len,
        node -> function.identifier -> identifier.ptr
    );

    fprintf(out, "    mov rsp, rbp\n");
    fprintf(out, "    pop rbp\n");
    fprintf(out, "    ret\n\n");
}

void gen_statement(Codegen* ctx, AstNode* node) {
    FILE* out = ctx -> output;
    
    switch (node -> kind) {
        case AST_VAR_DECL: {
            Symbol* sym = symbol_lookup(
                ctx -> sem_ctx -> symbols,
                node -> var_decl.identifier -> identifier.hash
            );

            ctx -> stack_offset -= sym -> type -> size;
            sym -> stack_offset = ctx -> stack_offset;

            if (node -> var_decl.value) {
                gen_expr(ctx, node -> var_decl.value);
                fprintf(out, "    mov [rbp%ld], rax\n", sym -> stack_offset);
            }
        } break;

        case AST_RETURN: {
            if (node -> return_stmt.expression) {
                gen_expr(ctx, node -> return_stmt.expression);
            }

            fprintf(
                out,
                "    jmp .end_%.*s\n",
                (int)ctx -> current_node -> function.identifier -> identifier.len,
                ctx -> current_node -> function.identifier -> identifier.ptr
            );
        } break;

        default: {
            gen_expr(ctx, node);
        } break;
    }
}

void gen_expr(Codegen* ctx, AstNode* expr) {
    FILE* out = ctx -> output;

    switch (expr -> kind) {
        case AST_LITERAL: {
            LiteralKind kind = expr -> literal.kind;

            if (kind == LIT_INT || kind == LIT_UINT) {
                i64 value = strtoll(expr -> literal.ptr, NULL, 10);
                fprintf(out, "    mov rax, %ld\n", value);
            } else if (kind == LIT_STRING) {
                u64 id = register_string_literal(
                    ctx,
                    expr -> literal.ptr + 1,
                    expr -> literal.len - 2
                );

                fprintf(out, "    lea rax, [rel str%lu]\n", id);
            }
        } break;

        case AST_IDENTIFIER: {
            Symbol* sym = symbol_lookup(ctx -> sem_ctx -> symbols, expr -> identifier.hash);
            fprintf(out, "    mov rax, [rbp%ld]\n", sym -> stack_offset);
        } break;

        case AST_BINARY: {
            gen_expr(ctx, expr -> binary.right);
            fprintf(out, "    push rax\n");

            gen_expr(ctx, expr -> binary.left);
            fprintf(out, "    pop rcx\n");

            switch (expr -> binary.op) {
                case T_PLUS: {
                    fprintf(out, "    add rax, rcx\n");
                } break;

                case T_MINUS: {
                    fprintf(out, "    sub rax, rcx\n");
                } break;

                case T_ASTERIX: {
                    fprintf(out, "    imul rax, rcx\n");
                } break;

                case T_SLASH: {
                    fprintf(out, "    div rax, rcx\n");
                } break;

                default: {
                    exit(2);
                }
            }
        } break;

        case AST_FN_CALL: {
            gen_fn_call(ctx, expr);
        } break;

        default: {
            exit(2);
        }
    }
}

void gen_fn_call(Codegen* ctx, AstNode* node) {
    u64 hash = node -> fn_call.identifier -> identifier.hash;
    
    if (hash == SYSCALL_HASH) {
        gen_syscall(ctx, node);
        return;
    }
}

void gen_syscall(Codegen* ctx, AstNode* node) {
    FILE* out = ctx -> output;

    const char* syscall_regs[] = {"rdi", "rsi", "rdx", "r10", "r8", "r9"};

    for (usize i = 0; i < node -> fn_call.args.count; i++) {
        gen_expr(ctx, node -> fn_call.args.nodes[i]);

        fprintf(out, "    push rax\n\n");
    }

    for (i64 i = node -> fn_call.args.count - 1; i >= 0; i--) {
        if (i == 0) {
            fprintf(out, "    pop rax\n");
        } else if (i <= 6) {
            fprintf(out, "    pop %s\n", syscall_regs[i - 1]);
        }
    }

    fprintf(out, "    syscall\n\n");
}

void gen_data_section(Codegen *ctx) {
    if (ctx -> string_count == 0) {
        return;
    }

    FILE* out = ctx -> output;

    fprintf(out, "section .data\n");

    for (usize i = 0; i < ctx -> string_count; i++) {
        StringLiteral* str = &ctx  ->  strings[i];
        
        fprintf(out, "str%lu: db \"", str -> id);
        fprintf(out, "%.*s", (i32) str -> len, str -> ptr);
        fprintf(out, "\", 0\n");
    }
}

u64 register_string_literal(Codegen* ctx, const char* ptr, usize len) {
    for (usize i = 0; i < ctx -> string_count; i++) {
        if (ctx -> strings[i].len == len && memcmp(ctx -> strings[i].ptr, ptr, len) == 0) {
            return ctx -> strings[i].id;
        }
    }
    
    if (ctx -> string_count >= ctx -> string_cap) {
        usize old_cap = ctx -> string_cap;
        usize new_cap = old_cap * 2;

        usize old_size = old_cap * sizeof(StringLiteral);
        usize new_size = new_cap * sizeof(StringLiteral);

        arena_realloc(ctx -> arena, ctx -> strings, old_size, new_size);
    }
    
    u64 id = ctx -> label_count++;
    
    ctx -> strings[ctx -> string_count++] = (StringLiteral) {
        .ptr = ptr,
        .len = len,
        .id = id,
    };
    
    return id;
}
