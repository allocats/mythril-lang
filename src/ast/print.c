#include <stdio.h>

#include "ast.h"
#include "types.h"

static void print_node(AstNode* node, int indent);
static void print_type(AstType* type);
static void print_slice(AstSlice slice);
static void print_indent(int level);

static void print_indent(int level) {
    for (int i = 0; i < level; i++) {
        printf("  ");
    }
}

static void print_slice(AstSlice slice) {
    printf("%.*s", (int)slice.len, slice.ptr);
}

static void print_type(AstType* type) {
    if (!type) {
        printf("<no-type>");
        return;
    }

    if (type -> is_const) {
        printf("CONST ");
    }
    
    switch (type->kind) {
        case TYPE_BASIC:
            print_slice(type->identifier);
            break;
        case TYPE_POINTER:
            print_type(type->pointee);
            printf("*");
            break;
        case TYPE_ARRAY:
            print_type(type->array.element_type);
            printf("[");
            if (type->array.size_expr) {
                print_node(type->array.size_expr, 0);
            }
            printf("]");
            break;
        case TYPE_ERR:
            printf("<error-type>");
            break;
    }
}

static void print_path(AstSlice* segments, usize count) {
    for (usize i = 0; i < count; i++) {
        print_slice(segments[i]);
        if (i < count - 1) printf("::");
    }
}

static const char* token_to_op_string(TokenKind kind) {
    switch (kind) {
        case TOK_PLUS: return "+";
        case TOK_MINUS: return "-";
        case TOK_STAR: return "*";
        case TOK_SLASH: return "/";
        case TOK_PERCENT: return "%";
        case TOK_EQUALS_EQUALS: return "==";
        case TOK_BANG_EQUALS: return "!=";
        case TOK_LESS_THAN: return "<";
        case TOK_LESS_THAN_EQUALS: return "<=";
        case TOK_GREATER_THAN: return ">";
        case TOK_GREATER_THAN_EQUALS: return ">=";
        case TOK_COND_AND: return "&&";
        case TOK_COND_OR: return "||";
        case TOK_BIT_AND_EQUALS: return "&";
        case TOK_BIT_OR: return "|";
        case TOK_BIT_XOR: return "^";
        case TOK_BIT_SHIFT_LEFT: return "<<";
        case TOK_BIT_SHIFT_RIGHT: return ">>";
        case TOK_BANG: return "!";
        case TOK_BIT_NOT: return "~";
        case TOK_AMPERSAND: return "&";
        case TOK_PLUS_PLUS: return "++";
        case TOK_MINUS_MINUS: return "--";
        case TOK_DOT: return ".";
        case TOK_ARROW: return "->";
        case TOK_EQUALS: return "=";
        case TOK_PLUS_EQUALS: return "+=";
        case TOK_MINUS_EQUALS: return "-=";
        case TOK_STAR_EQUALS: return "*=";
        case TOK_SLASH_EQUALS: return "/=";
        case TOK_PERCENT_EQUALS: return "%=";
        case TOK_BIT_OR_EQUALS: return "|=";
        case TOK_BIT_XOR_EQUALS: return "^=";
        case TOK_BIT_SHIFT_LEFT_EQUALS: return "<<=";
        case TOK_BIT_SHIFT_RIGHT_EQUALS: return ">>=";
        default: return "<unknown-op>";
    }
}

static void print_node(AstNode* node, int indent) {
    if (!node) return;
    
    print_indent(indent);
    
    switch (node->kind) {
        case AST_MODULE_DECL: {
            printf("MODULE ");
            print_path(node->module_decl.segments, node->module_decl.count);
            printf("\n");
            break;
        }
        
        case AST_IMPORT_DECL: {
            printf("IMPORT ");
            print_path(node->import_decl.segments, node->import_decl.count);
            printf("\n");
            break;
        }
        
        case AST_STRUCT_DECL: {
            printf("STRUCT ");
            print_slice(node->struct_decl.identifier);
            printf(" {\n");
            for (usize i = 0; i < node->struct_decl.count; i++) {
                print_indent(indent + 1);
                print_slice(node->struct_decl.fields[i]->identifier);
                printf(": ");
                print_type(node->struct_decl.fields[i]->type);
                printf("\n");
            }
            print_indent(indent);
            printf("}\n");
            break;
        }
        
        case AST_ENUM_DECL: {
            printf("ENUM ");
            print_slice(node->enum_decl.identifier);
            printf(" {\n");
            for (usize i = 0; i < node->enum_decl.count; i++) {
                print_indent(indent + 1);
                print_slice(node->enum_decl.variants[i]->identifier);
                if (node->enum_decl.variants[i]->value) {
                    printf(" = (\n");
                    print_node(node->enum_decl.variants[i]->value, indent+2);
                    print_indent(indent + 1);
                    printf(")");
                }
                printf("\n");
            }
            print_indent(indent);
            printf("}\n");
            break;
        }
        
        case AST_IMPL_DECL: {
            printf("IMPL ");
            print_slice(node->impl_decl.target);
            printf(" {\n");
            for (usize i = 0; i < node->impl_decl.fn_count; i++) {
                print_node(node->impl_decl.functions[i], indent + 1);
            }
            print_indent(indent);
            printf("}\n");
            break;
        }
        
        case AST_FUNCTION_DECL: {
            printf("FN ");
            print_slice(node->function_decl.identifier);
            printf("(");
            for (usize i = 0; i < node->function_decl.param_count; i++) {
                print_slice(node->function_decl.parameters[i].identifier);
                printf(": ");
                print_type(node->function_decl.parameters[i].type);
                if (i < node->function_decl.param_count - 1) printf(", ");
            }
            printf("): ");
            print_type(node->function_decl.return_type);
            printf(" {\n");
            for (usize i = 0; i < node->function_decl.stmt_count; i++) {
                print_node(node->function_decl.statements[i], indent + 1);
            }
            print_indent(indent);
            printf("}\n");
            break;
        }
        
        case AST_STATIC_DECL: {
            printf("STATIC ");
            print_slice(node->static_decl.identifier);
            printf(": ");
            print_type(node->static_decl.type);
            if (node->static_decl.value) {
                printf(" = ");
                print_node(node->static_decl.value, 0);
            }
            printf("\n");
            break;
        }
        
        case AST_CONST_DECL: {
            printf("CONST ");
            print_slice(node->const_decl.identifier);
            printf(": ");
            print_type(node->const_decl.type);
            printf(" = ");
            print_node(node->const_decl.value, 0);
            printf("\n");
            break;
        }
        
        case AST_VAR_DECL: {
            printf("VAR ");
            print_slice(node->var_decl.identifier);
            printf(": ");
            print_type(node->var_decl.type);
            if (node->var_decl.value) {
                printf(" = ");
                print_node(node->var_decl.value, 0);
                printf("\n");
            } else {
                printf("\n");
            }
            break;
        }
        
        case AST_ASSIGNMENT: {
            printf("ASSIGN ");
            print_node(node->assignment.lvalue, 0);
            printf(" %s ", token_to_op_string(node->assignment.op));
            print_node(node->assignment.rvalue, 0);
            printf("\n");
            break;
        }
        
        case AST_IF_STMT: {
            printf("IF ");
            print_node(node->if_stmt.expression, 0);
            printf(" {\n");
            for (usize i = 0; i < node->if_stmt.stmt_count; i++) {
                print_node(node->if_stmt.statements[i], indent + 1);
            }
            print_indent(indent);
            printf("}");
            if (node->if_stmt.else_stmt) {
                printf(" ELSE ");
                if (node->if_stmt.else_stmt->kind == AST_IF_STMT) {
                    printf("\n");
                    print_node(node->if_stmt.else_stmt, indent);
                } else {
                    printf("{\n");
                    print_node(node->if_stmt.else_stmt, indent + 1);
                    print_indent(indent);
                    printf("}\n");
                }
            } else {
                printf("\n");
            }
            break;
        }
        
        case AST_MATCH_STMT: {
            printf("MATCH ");
            print_node(node->match_stmt.expression, 0);
            printf(" {\n");
            for (usize i = 0; i < node->match_stmt.arm_count; i++) {
                print_indent(indent + 1);
                print_node(node->match_stmt.arms[i].pattern, 0);
                printf(": {\n");
                for (usize j = 0; j < node->match_stmt.arms[i].stmt_count; j++) {
                    print_node(node->match_stmt.arms[i].statements[j], indent + 2);
                }
                print_indent(indent + 1);
                printf("}\n");
            }
            if (node->match_stmt.stmt_count > 0) {
                print_indent(indent + 1);
                printf("_: {\n");
                for (usize i = 0; i < node->match_stmt.stmt_count; i++) {
                    print_node(node->match_stmt.statements[i], indent + 2);
                }
                print_indent(indent + 1);
                printf("}\n");
            }
            print_indent(indent);
            printf("}\n");
            break;
        }
        
        case AST_LOOP_STMT: {
            printf("LOOP {\n");
            for (usize i = 0; i < node->loop_stmt.stmt_count; i++) {
                print_node(node->loop_stmt.statements[i], indent + 1);
            }
            print_indent(indent);
            printf("}\n");
            break;
        }
        
        case AST_WHILE_STMT: {
            printf("WHILE ");
            print_node(node->while_stmt.cond, 0);
            printf(" {\n");
            for (usize i = 0; i < node->while_stmt.stmt_count; i++) {
                print_node(node->while_stmt.statements[i], indent + 1);
            }
            print_indent(indent);
            printf("}\n");
            break;
        }
        
        case AST_FOR_STMT: {
            printf("FOR ");
            print_node(node->for_stmt.init, 0);
            printf("; ");
            print_node(node->for_stmt.cond, 0);
            printf("; ");
            print_node(node->for_stmt.step, 0);
            printf(" {\n");
            for (usize i = 0; i < node->for_stmt.stmt_count; i++) {
                print_node(node->for_stmt.statements[i], indent + 1);
            }
            print_indent(indent);
            printf("}\n");
            break;
        }
        
        case AST_BREAK_STMT: {
            printf("BREAK\n");
            break;
        }
        
        case AST_CONTINUE_STMT: {
            printf("CONTINUE\n");
            break;
        }
        
        case AST_RETURN_STMT: {
            printf("RETURN");
            if (node->return_stmt.expression) {
                printf(" ");
                print_node(node->return_stmt.expression, 0);
                printf("\n");
                print_node(node->return_stmt.expression, indent + 1);
            } else {
                printf("\n");
            }
            break;
        }
        
        case AST_EXPR_STMT: {
            print_node(node->expr_stmt.expression, indent);
            break;
        }
        
        case AST_UNARY: {
            printf("Unary(%s)\n", token_to_op_string(node->unary.op));
            print_node(node->unary.operand, indent + 1);
            break;
        }
        
        case AST_BINARY: {
            printf("Binary(%s)\n", token_to_op_string(node->binary.op));
            print_node(node->binary.left, indent + 1);
            print_node(node->binary.right, indent + 1);
            break;
        }
        
        case AST_FUNCTION_CALL: {
            printf("Call(");
            print_slice(node->function_call.identifier);
            printf(")\n");
            for (usize i = 0; i < node->function_call.arg_count; i++) {
                print_node(node->function_call.arguments[i], indent + 1);
            }
            break;
        }
        
        case AST_ARRAY_INDEX: {
            printf("ArrayIndex\n");
            print_node(node->array_index.array, indent + 1);
            print_node(node->array_index.index, indent + 1);
            break;
        }
        
        case AST_MEMBER_ACCESS: {
            printf("Member(%s", token_to_op_string(node->member_access.op));
            print_slice(node->member_access.member);
            printf(")\n");
            print_node(node->member_access.object, indent + 1);
            break;
        }
        
        case AST_IDENTIFIER: {
            printf("Identifier(");
            print_slice(node->identifier.value);
            printf(")\n");
            break;
        }
        
        case AST_LITERAL: {
            printf("Literal(");
            print_slice(node->literal.value);
            printf(")\n");
            break;
        }
        
        case AST_PATTERN_IDENT: {
            print_slice(node->pattern_ident.identifier);
            break;
        }
        
        case AST_PATTERN_LITERAL: {
            print_slice(node->pattern_literal.literal.value);
            break;
        }
        
        case AST_PATTERN_VARIANT: {
            print_slice(node->pattern_variant.variant);
            if (node->pattern_variant.count > 0) {
                printf("(");
                for (usize i = 0; i < node->pattern_variant.count; i++) {
                    print_node(node->pattern_variant.patterns[i], 0);
                    if (i < node->pattern_variant.count - 1) printf(", ");
                }
                printf(")");
            }
            break;
        }
        
        case AST_PATTERN_WILDCARD: {
            printf("_");
            break;
        }
        
        case AST_ERROR: {
            printf("<ERROR>\n");
            break;
        }
    }
}

void print_program(Program* program) {
    if (!program) {
        printf("NULL program\n");
        return;
    }

    printf("[AST Start]\n\n");
    
    for (usize i = 0; i < program->count; i++) {
        print_node(program->declarations[i], 0);

        if (i < program->count - 1) {
            printf("\n");
        }
    }

    printf("\n[AST End]\n");
}
