#include <stdio.h>

#include "types.h"

void print_ast_node(AstNode* node, int indent);

static void print_indent(int indent) {
    for (int i = 0; i < indent; i++) printf("    ");
}

static void print_slice(AstSlice s) {
    printf("%.*s", (int)s.len, s.ptr);
}

static void print_type(AstType* t, int indent);

static void print_type(AstType* t, int indent) {
    if (!t) {
        printf("<null type>");
        return;
    }

    switch (t->kind) {
        case TYPE_BASIC:
            print_slice(t->identifier);
            break;

        case TYPE_POINTER:
            printf("*");
            print_type(t->pointee, indent);
            break;

        case TYPE_ARRAY:
            printf("[");
            if (t->array.size_expr) {
                // simplistic
                printf("size_expr");
            }
            printf("] ");
            print_type(t->array.element_type, indent);
            break;

        default:
            printf("<unknown type>");
    }
}

static void print_ast_vec(AstVec* v, int indent) {
    for (usize i = 0; i < v->count; i++) {
        AstNode* n = v->items[i];
        print_ast_node(n, indent);
    }
}

void print_ast_node(AstNode* node, int indent) {
    if (!node) {
        print_indent(indent);
        printf("<null node>\n");
        return;
    }

    print_indent(indent);
    printf("%s", AST_KIND_STRINGS[node->kind]);

    /* If the node can be followed by a line break, print it */
    printf(":\n");

    switch (node->kind) {

    /* ────────────────────────────────
       DECLARATIONS
       ──────────────────────────────── */
    case AST_MODULE_DECL: {
        print_indent(indent+1);
        printf("module ");
        for (usize i = 0; i < node->module_decl.count; i++) {
            print_slice(node->module_decl.segments[i]);
            if (i + 1 < node->module_decl.count) printf("::");
        }
        printf("\n");
    } break;

    case AST_IMPORT_DECL: {
        print_indent(indent+1);
        printf("import ");
        for (usize i = 0; i < node->import_decl.count; i++) {
            print_slice(node->import_decl.segments[i]);
            if (i + 1 < node->import_decl.count) printf("::");
        }
        printf("\n");
    } break;

    case AST_STRUCT_DECL: {
        print_indent(indent+1);
        printf("struct ");
        print_slice(node->struct_decl.identifier);
        printf(" {\n");

        for (usize i = 0; i < node->struct_decl.count; i++) {
            AstStructField* f = &node->struct_decl.fields[i];
            print_indent(indent+2);
            print_slice(f->identifier);
            printf(": ");
            print_type(f->type, indent);
            printf("\n");
        }

        print_indent(indent+1);
        printf("}\n");
    } break;

    case AST_ENUM_DECL: {
        print_indent(indent+1);
        printf("enum ");
        print_slice(node->enum_decl.identifier);
        printf(" {\n");

        for (usize i = 0; i < node->enum_decl.count; i++) {
            AstEnumVariant* v = &node->enum_decl.variants[i];
            print_indent(indent+2);
            print_slice(v->identifier);

            if (v->count > 0) {
                printf("(");
                for (usize j = 0; j < v->count; j++) {
                    print_type(v->types[j], indent+2);
                    if (j + 1 < v->count) printf(", ");
                }
                printf(")");
            }
            printf("\n");
        }

        print_indent(indent+1);
        printf("}\n");
    } break;

    case AST_IMPL_DECL: {
        print_indent(indent+1);
        printf("impl ");
        print_slice(node->impl_decl.target);
        printf(" {\n");

        print_ast_vec(&node->impl_decl.functions, indent+2);

        print_indent(indent+1);
        printf("}\n");
    } break;

    case AST_FUNCTION_DECL: {
        print_indent(indent+1);
        printf("fn ");
        print_slice(node->function_decl.identifier);
        printf("(");
        for (usize i = 0; i < node->function_decl.count; i++) {
            AstParameter* p = &node->function_decl.parameters[i];
            print_slice(p->identifier);
            printf(": ");
            print_type(p->type, indent);
            if (i + 1 < node->function_decl.count) printf(", ");
        }
        printf(")");

        if (node->function_decl.return_type) {
            printf(" -> ");
            print_type(node->function_decl.return_type, indent);
        }

        printf(" {\n");
        print_ast_vec(&node->function_decl.block, indent+2);
        print_indent(indent+1);
        printf("}\n");
    } break;

    case AST_CONST_DECL: {
        print_indent(indent+1);
        printf("const ");
        print_slice(node->const_decl.identifier);
        printf(": ");
        print_type(node->const_decl.type, indent);
        printf(" =\n");
        print_ast_node(node->const_decl.value, indent+2);
    } break;

    case AST_VAR_DECL: {
        print_indent(indent+1);
        printf("var ");
        print_slice(node->var_decl.identifier);
        printf(": ");
        print_type(node->var_decl.type, indent);

        if (node->var_decl.value) {
            printf(" =\n");
            print_ast_node(node->var_decl.value, indent+2);
        } else {
            printf("\n");
        }
    } break;

    case AST_STATIC_DECL: {
        print_indent(indent+1);
        printf("static ");
        print_slice(node->static_decl.identifier);
        printf(": ");
        print_type(node->static_decl.type, indent);
        printf(" =\n");
        print_ast_node(node->static_decl.value, indent+2);
    } break;

    /* ────────────────────────────────
       STATEMENTS
       ──────────────────────────────── */
    case AST_ASSIGNMENT: {
        print_indent(indent+1);
        printf("assignment:\n");
        print_ast_node(node->assignment.lvalue, indent+2);
        print_indent(indent+2);
        printf("op: %s\n", TOKEN_KIND_STRINGS[node->assignment.op]);
        print_ast_node(node->assignment.rvalue, indent+2);
    } break;

    case AST_IF_STMT: {
        print_indent(indent+1);
        printf("if:\n");
        print_ast_node(node->if_stmt.expression, indent+2);

        print_indent(indent+1);
        printf("then:\n");
        print_ast_vec(&node->if_stmt.then_block, indent+2);

        if (node->if_stmt.else_stmt) {
            print_indent(indent+1);
            printf("else:\n");
            print_ast_node(node->if_stmt.else_stmt, indent+2);
        }
    } break;

    case AST_MATCH_STMT: {
        print_indent(indent+1);
        printf("match:\n");
        print_ast_node(node->match_stmt.expression, indent+2);

        for (usize i = 0; i < node->match_stmt.count; i++) {
            AstMatchArm* arm = &node->match_stmt.arms[i];
            print_indent(indent+1);
            printf("arm:\n");

            print_ast_node(arm->pattern, indent+2);
            print_ast_vec(&arm->block, indent+2);
        }

        if (node->match_stmt.default_block.count > 0) {
            print_indent(indent+1);
            printf("default:\n");
            print_ast_vec(&node->match_stmt.default_block, indent+2);
        }
    } break;

    case AST_LOOP_STMT: {
        print_indent(indent+1);
        printf("loop:\n");
        print_indent(indent+2);
        printf("block:\n");
        print_ast_vec(&node->loop_stmt.block, indent+3);
    } break;

    case AST_BREAK_STMT:
        print_indent(indent+1);
        printf("break\n");
        break;

    case AST_CONTINUE_STMT:
        print_indent(indent+1);
        printf("continue\n");
        break;

    case AST_RETURN_STMT:
        print_indent(indent+1);
        printf("return:\n");
        if (node->return_stmt.expression)
            print_ast_node(node->return_stmt.expression, indent+2);
        break;

    case AST_EXPR_STMT:
        print_indent(indent+1);
        printf("expr-stmt:\n");
        print_ast_node(node->expr_stmt.expression, indent+2);
        break;

    /* ────────────────────────────────
       EXPRESSIONS
       ──────────────────────────────── */

    case AST_UNARY:
        print_indent(indent+1);
        printf("unary op=%s postfix=%d\n", TOKEN_KIND_STRINGS[node->unary.op], node->unary.is_postfix);
        print_ast_node(node->unary.operand, indent+2);
        break;

    case AST_BINARY:
        print_indent(indent+1);
        printf("binary op=%s\n", TOKEN_KIND_STRINGS[node->binary.op]);
        print_ast_node(node->binary.left, indent+2);
        print_ast_node(node->binary.right, indent+2);
        break;

    case AST_FUNCTION_CALL:
        print_indent(indent+1);
        printf("call ");
        print_slice(node->function_call.identifier);
        printf("(\n");
        print_ast_vec(&node->function_call.args, indent+2);
        print_indent(indent+1);
        printf(")\n");
        break;

    case AST_ARRAY_INDEX:
        print_indent(indent+1);
        printf("array-index:\n");
        print_ast_node(node->array_index.array, indent+2);
        print_ast_node(node->array_index.index, indent+2);
        break;

    case AST_MEMBER_ACCESS:
        print_indent(indent+1);
        printf("member-access .");
        print_slice(node->member_access.member);
        printf("\n");
        print_ast_node(node->member_access.object, indent+2);
        break;

    case AST_IDENTIFIER:
        print_indent(indent+1);
        printf("id ");
        print_slice(node->identifier.value);
        printf("\n");
        break;

    case AST_LITERAL:
        print_indent(indent+1);
        printf("literal %d: ", node->literal.kind);
        print_slice(node->literal.value);
        printf("\n");
        break;

    /* ────────────────────────────────
       PATTERNS
       ──────────────────────────────── */
    case AST_PATTERN_IDENT:
        print_indent(indent+1);
        printf("pattern ident ");
        print_slice(node->pattern_ident.identifier);
        printf("\n");
        break;

    case AST_PATTERN_LITERAL:
        print_indent(indent+1);
        printf("pattern literal:\n");
        print_indent(indent+2);
        print_slice(node->pattern_literal.literal.value);
        printf("\n");
        break;

    case AST_PATTERN_VARIANT:
        print_indent(indent+1);
        printf("pattern variant ");
        print_slice(node->pattern_variant.variant);
        printf("(\n");
        print_ast_vec(&node->pattern_variant.patterns, indent+2);
        print_indent(indent+1);
        printf(")\n");
        break;

    /* ──────────────────────────────── */

    default:
        print_indent(indent+1);
        printf("<unhandled AST kind>\n");
        break;
    }
}

void print_program(Program* p) {
    for (usize i = 0; i < p->count; i++)
        print_ast_node(&p->items[i], 0);
}
