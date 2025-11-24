#include "ast.h"
#include "types.h"
#include "parser/parser.h"

#include "../hash/hash.h"
#include "../utils/macros.h"
#include <stdio.h>

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

    program -> nodes = arena_alloc(arena, sizeof(AstNode) * capacity);
    program -> count = 0;
    program -> capacity = capacity;

    while (parser.index < parser.count) {
        AstNode* node = nullptr;
        Token token = *parser_peek(&parser); 
        
        switch (token.type) {
            case T_FUNCTION: {
                node = parse_function_decl(&parser);
            } break;

            // case T_MODULE: {
            //     node = parse_module(&parser);
            // } break;
            //
            // case T_IMPORT: {
            //     node = parse_import(&parser);
            // } break;
            //
            // case T_STRUCT: {
            //     node = parse_struct_decl(&parser);
            // } break;
            //
            // case T_ENUM: {
            //     node = parse_enum_decl(&parser);
            // } break;
            //
            // case T_IMPL: {
            //     node = parse_impl_decl(&parser);
            // } break;
            //
            // case T_STATIC: {
            //     node = parse_static_decl(&parser);
            // } break;
            //
            // case T_CONST: {
            //     node = parse_const_decl(&parser);
            // } break;

            default: {
                if (parser.index != (parser.count - 1)) {
                    ast_error(&token, buf, "Parsing bug, unknown token");
                } else {
                    return program;
                }
            } break;
        }

        if (node) {
            program -> nodes[program -> count++] = *node;
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

inline
void ast_vec_push(ArenaAllocator* arena, AstVec* vec, AstNode* node) {
    MEOW_LOG("Pushing node (%p) into vec (%p)\n", node, vec);
    usize old_cap = vec -> cap;

    if (vec -> count >= old_cap) {
        usize new_cap = old_cap == 0 ? 8 : old_cap * 2;

        usize old_size = old_cap * sizeof(AstNode*);
        usize new_size = new_cap * sizeof(AstNode*);

        vec -> nodes = arena_realloc(arena, vec -> nodes, old_size, new_size);
        vec -> cap = new_cap;
    }

    vec -> nodes[vec -> count++] = node;
}

static void print_node(AstNode* node, int indent);
static void print_indent(int indent);

// Helper to print indentation
static void print_indent(int indent) {
    for (int i = 0; i < indent; i++) {
        printf("  ");
    }
}

// Helper to print AstVec
static void print_ast_vec(AstVec* vec, int indent, const char* label) {
    if (vec->count > 0) {
        print_indent(indent);
        printf("%s (%zu nodes):\n", label, vec->count);
        for (usize i = 0; i < vec->count; i++) {
            print_node(vec->nodes[i], indent + 1);
        }
    }
}

// Print a single AST node
static void print_node(AstNode* node, int indent) {
    if (!node) {
        print_indent(indent);
        printf("<null> %d\n", __LINE__);
        return;
    } 

    print_indent(indent);
    printf("%s", AST_KIND_STRINGS[node->kind]);

    switch (node->kind) {
        case AST_MODULE:
            printf("\n");
            print_node(node->module.identifier, indent + 1);
            break;

        case AST_IMPORT:
            printf("\n");
            print_node(node->import.module, indent + 1);
            break;

        case AST_MACRO:
            printf("\n");
            print_node(node->macro_decl.identifier, indent + 1);
            print_ast_vec(&node->macro_decl.params, indent + 1, "Params");
            print_ast_vec(&node->macro_decl.block, indent + 1, "Block");
            break;

        case AST_STRUCT:
            printf("\n");
            print_node(node->struct_decl.identifier, indent + 1);
            print_ast_vec(&node->struct_decl.fields, indent + 1, "Fields");
            break;

        case AST_ENUM:
            printf("\n");
            print_node(node->enum_decl.identifier, indent + 1);
            if (node->enum_decl.enum_count > 0) {
                print_indent(indent + 1);
                printf("Values (%zu):\n", node->enum_decl.enum_count);
                for (usize i = 0; i < node->enum_decl.enum_count; i++) {
                    print_node(node->enum_decl.values[i], indent + 2);
                }
            }
            break;

        case AST_IMPL:
            printf("\n");
            print_node(node->impl.type_name, indent + 1);
            print_ast_vec(&node->impl.functions, indent + 1, "Functions");
            break;

        case AST_FUNCTION:
            printf("\n");
            print_indent(indent + 1);
            printf("Return Type:\n");
            print_node(node->function.return_type, indent + 2);
            print_node(node->function.identifier, indent + 1);
            print_ast_vec(&node->function.params, indent + 1, "Params");
            print_ast_vec(&node->function.block, indent + 1, "Block");
            break;

        case AST_VAR_DECL:
            printf(" (ptr_depth: %zu)\n", node->var_decl.pointer_depth);
            print_indent(indent + 1);
            printf("Type:\n");
            print_node(node->var_decl.type, indent + 2);
            print_indent(indent + 1);
            printf("Identifier:\n");
            print_node(node->var_decl.identifier, indent + 2);
            if (node->var_decl.value) {
                print_indent(indent + 1);
                printf("Value:\n");
                print_node(node->var_decl.value, indent + 2);
            }
            break;

        case AST_CONST_DECL:
            printf("\n");
            print_indent(indent + 1);
            printf("Type:\n");
            print_node(node->const_decl.type, indent + 2);
            print_indent(indent + 1);
            printf("Identifier:\n");
            print_node(node->const_decl.identifier, indent + 2);
            print_indent(indent + 1);
            printf("Value:\n");
            print_node(node->const_decl.value, indent + 2);
            break;

        case AST_ASSIGNMENT:
            printf(" (%s)\n", OPKIND_STRINGS[node->assignment.op]);
            print_indent(indent + 1);
            printf("Target:\n");
            print_node(node->assignment.target, indent + 2);
            print_indent(indent + 1);
            printf("Value:\n");
            print_node(node->assignment.value, indent + 2);
            break;

        case AST_EXPR_STMT:
            printf("\n");
            // Assuming expr_stmt wraps another expression node
            break;

        case AST_IF:
            printf("\n");
            print_indent(indent + 1);
            printf("Condition:\n");
            print_node(node->if_stmt.cond, indent + 2);
            print_ast_vec(&node->if_stmt.if_block, indent + 1, "If Block");
            print_ast_vec(&node->if_stmt.else_block, indent + 1, "Else Block");
            break;

        case AST_FOR:
            printf("\n");
            if (node->for_loop.initial) {
                print_indent(indent + 1);
                printf("Initial:\n");
                print_node(node->for_loop.initial, indent + 2);
            }
            if (node->for_loop.condition) {
                print_indent(indent + 1);
                printf("Condition:\n");
                print_node(node->for_loop.condition, indent + 2);
            }
            if (node->for_loop.step) {
                print_indent(indent + 1);
                printf("Step:\n");
                print_node(node->for_loop.step, indent + 2);
            }
            print_ast_vec(&node->for_loop.block, indent + 1, "Block");
            break;

        case AST_RETURN:
            printf("\n");
            if (node->return_stmt.expression) {
                print_node(node->return_stmt.expression, indent + 1);
            }
            break;

        case AST_INDEX:
            printf("\n");
            print_indent(indent + 1);
            printf("Array:\n");
            print_node(node->index.array, indent + 2);
            print_indent(indent + 1);
            printf("Index:\n");
            print_node(node->index.index, indent + 2);
            break;

        case AST_MEMBER_ACCESS:
            printf(" .%.*s\n", (int)node->member_access.member_len, 
                   node->member_access.member_ptr);
            print_indent(indent + 1);
            printf("Object:\n");
            print_node(node->member_access.object, indent + 2);
            break;

        case AST_UNARY:
            printf(" (op: %d)\n", node->unary.op);
            print_node(node->unary.operand, indent + 1);
            break;

        case AST_BINARY:
            printf(" (op: %d)\n", node->binary.op);
            print_indent(indent + 1);
            printf("Left:\n");
            print_node(node->binary.left, indent + 2);
            print_indent(indent + 1);
            printf("Right:\n");
            print_node(node->binary.right, indent + 2);
            break;

        case AST_FN_CALL:
            printf("\n");
            print_node(node->fn_call.identifier, indent + 1);
            print_ast_vec(&node->fn_call.args, indent + 1, "Arguments");
            break;

        case AST_LITERAL:
            printf(" (%s): %.*s\n", 
                   LITERAL_STRINGS[node->literal.kind],
                   (int)node->literal.len, 
                   node->literal.ptr);
            break;

        case AST_POSTFIX:
            printf(" (op: %d)\n", node->postfix.op);
            print_node(node->postfix.operand, indent + 1);
            break;

        case AST_IDENTIFIER:
            printf(": %.*s (hash: %lu)\n", 
                   (int)node->identifier.len, 
                   node->identifier.ptr,
                   node->identifier.hash);
            break;

        default:
            printf(" <unknown node type>\n");
            break;
    }
}

// Main function to print the entire program
void print_program(Program* program) {
    if (!program) {
        printf("Program is NULL\n");
        return;
    }

    printf("=== Program AST (%zu nodes) ===\n\n", program->count);
    
    for (usize i = 0; i < program->count; i++) {
        printf("--- Node %zu ---\n", i);
        print_node(&program->nodes[i], 0);
        printf("\n");
    }
    
    printf("=== End of Program ===\n");
}
