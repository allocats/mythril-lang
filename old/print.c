// #include <stdio.h>
//
// #include "ast.h"
// #include "types.h"
//
// #define println(fmt, ...) \
//     fprintf(stdout, fmt "\n", ##__VA_ARGS__);
//
// void print_node(AstNode* node, u32 indent);
//
// void print_indent(u32 indent) {
//     for (u32 i = 0; i < indent; i++) {
//         printf("    ");
//     }
// }
//
// void print_slice(AstSlice slice) {
//     printf("%.*s", (u32) slice.len, slice.ptr);
// }
//
// /* ----------------------------------------
//         TYPE PRINTING
// ---------------------------------------- */
//
// void print_type(AstType* type) {
//     if (!type) {
//         printf("<null type>");
//         return;
//     }
//
//     switch (type->kind) {
//         case TYPE_ERR:
//             printf("<type error>");
//             break;
//
//         case TYPE_BASIC:
//             print_slice(type->identifier);
//             break;
//
//         case TYPE_POINTER:
//             printf("*");
//             print_type(type->pointee);
//             break;
//
//         case TYPE_ARRAY:
//             printf("[");
//             print_node(type->array.size_expr, 0);
//             printf("] ");
//             print_type(type->array.element_type);
//             break;
//
//         default:
//             printf("<unknown type>");
//             break;
//     }
// }
//
// /* ----------------------------------------
//         NODE & LIST HELPERS
// ---------------------------------------- */
//
// void print_node_list(AstNode** list, usize count, u32 indent) {
//     for (usize i = 0; i < count; i++) {
//         print_node(list[i], indent);
//     }
// }
//
// void print_pattern_list(AstNode** list, usize count, u32 indent) {
//     for (usize i = 0; i < count; i++) {
//         print_node(list[i], indent);
//     }
// }
//
// /* ----------------------------------------
//         MAIN AST NODE PRINTER
// ---------------------------------------- */
//
// void print_node(AstNode* node, u32 indent) {
//     if (!node) {
//         print_indent(indent);
//         println("<null node>");
//         return;
//     }
//
//     // print_indent(indent);
//     println("[Node: %s] {", AST_KIND_STRINGS[node->kind]);
//
//     print_indent(indent + 1);
//
//     switch (node->kind) {
//
// /* ----------------------------------------
//         DECLARATIONS
// ---------------------------------------- */
//
//         case AST_MODULE_DECL: {
//             printf("module: ");
//             for (usize i = 0; i < node->module_decl.count; i++) {
//                 print_slice(node->module_decl.segments[i]);
//                 if (i + 1 < node->module_decl.count) printf("::");
//             }
//         } break;
//
//         case AST_IMPORT_DECL: {
//             printf("import: ");
//             for (usize i = 0; i < node->import_decl.count; i++) {
//                 print_slice(node->import_decl.segments[i]);
//                 if (i + 1 < node->import_decl.count) printf("::");
//             }
//         } break;
//
//         case AST_STRUCT_DECL: {
//             printf("struct %.*s", 
//                 (int)node->struct_decl.identifier.len, 
//                 node->struct_decl.identifier.ptr
//             );
//             println("");
//
//             for (usize i = 0; i < node->struct_decl.count; i++) {
//                 print_indent(indent + 2);
//                 print_slice(node->struct_decl.fields[i].identifier);
//                 printf(": ");
//                 print_type(node->struct_decl.fields[i].type);
//                 println("");
//             }
//         } break;
//
//         case AST_ENUM_DECL: {
//             printf("enum %.*s", 
//                 (int)node->enum_decl.identifier.len,
//                 node->enum_decl.identifier.ptr
//             );
//             println("");
//
//             for (usize i = 0; i < node->enum_decl.count; i++) {
//                 AstEnumVariant* v = &node->enum_decl.variants[i];
//                 print_indent(indent + 2);
//                 print_slice(v->identifier);
//                 if (v->count > 0) {
//                     printf("(");
//                     for (usize j = 0; j < v->count; j++) {
//                         print_type(v->types[j]);
//                         if (j + 1 < v->count) printf(", ");
//                     }
//                     printf(")");
//                 }
//                 println("");
//             }
//         } break;
//
//         case AST_IMPL_DECL: {
//             printf("impl ");
//             print_slice(node->impl_decl.target);
//             println("");
//
//             print_node_list(node->impl_decl.functions,
//                             node->impl_decl.fn_count,
//                             indent + 2);
//         } break;
//
//         case AST_FUNCTION_DECL: {
//             printf("fn %.*s(", 
//                 (int)node->function_decl.identifier.len,
//                 node->function_decl.identifier.ptr
//             );
//
//             for (usize i = 0; i < node->function_decl.param_count; i++) {
//                 AstParameter* p = &node->function_decl.parameters[i];
//                 print_slice(p->identifier);
//                 printf(": ");
//                 print_type(p->type);
//                 if (i + 1 < node->function_decl.param_count) printf(", ");
//             }
//
//             printf(") -> ");
//             print_type(node->function_decl.return_type);
//             println("");
//
//             print_node_list(node->function_decl.statements,
//                             node->function_decl.stmt_count,
//                             indent + 2);
//         } break;
//
//         case AST_CONST_DECL: {
//             printf("const ");
//             print_slice(node->const_decl.identifier);
//             printf(": ");
//             print_type(node->const_decl.type);
//             printf(" = ");
//             print_node(node->const_decl.value, 0);
//         } break;
//
//         case AST_STATIC_DECL: {
//             printf("static ");
//             print_slice(node->static_decl.identifier);
//             printf(": ");
//             print_type(node->static_decl.type);
//             printf(" = ");
//             print_node(node->static_decl.value, 0);
//         } break;
//
//         case AST_VAR_DECL: {
//             printf("var ");
//             print_slice(node->var_decl.identifier);
//             printf(": ");
//             print_type(node->var_decl.type);
//
//             if (node->var_decl.value) {
//                 printf(" = ");
//                 print_node(node->var_decl.value, 0);
//             }
//         } break;
//
// /* ----------------------------------------
//         STATEMENTS
// ---------------------------------------- */
//
//         case AST_ASSIGNMENT: {
//             printf("assign ");
//             print_node(node->assignment.lvalue, 0);
//             printf(" %d ", node->assignment.op);
//             print_node(node->assignment.rvalue, 0);
//         } break;
//
//         case AST_IF_STMT: {
//             printf("if ");
//             print_node(node->if_stmt.expression, 0);
//             println("");
//
//             print_node_list(node->if_stmt.statements,
//                             node->if_stmt.stmt_count,
//                             indent + 2);
//
//             if (node->if_stmt.else_stmt) {
//                 print_indent(indent + 1);
//                 println("else");
//                 print_node(node->if_stmt.else_stmt, indent + 2);
//             }
//         } break;
//
//         case AST_MATCH_STMT: {
//             printf("match ");
//             print_node(node->match_stmt.expression, 0);
//             println("");
//
//             for (usize i = 0; i < node->match_stmt.arm_count; i++) {
//                 AstMatchArm* arm = &node->match_stmt.arms[i];
//                 print_indent(indent + 2);
//                 print_node(arm->pattern, 0);
//                 println("");
//
//                 print_node_list(arm->statements, arm->stmt_count, indent + 3);
//             }
//
//             print_indent(indent + 1);
//             println("_:");
//             print_node_list(node->match_stmt.statements,
//                             node->match_stmt.stmt_count,
//                             indent + 2);
//         } break;
//
//         case AST_LOOP_STMT: {
//             printf("loop\n");
//             print_node_list(node->loop_stmt.statements,
//                             node->loop_stmt.stmt_count,
//                             indent + 2);
//         } break;
//
//         case AST_WHILE_STMT: {
//             printf("while ");
//             print_node(node->while_stmt.cond, 0);
//             println("");
//
//             print_node_list(node->while_stmt.statements,
//                             node->while_stmt.stmt_count,
//                             indent + 2);
//         } break;
//
//         case AST_FOR_STMT: {
//             printf("for ");
//             print_node(node->for_stmt.init, 0);
//             printf("; ");
//             print_node(node->for_stmt.cond, 0);
//             printf("; ");
//             print_node(node->for_stmt.step, 0);
//             println("");
//
//             print_node_list(node->for_stmt.statements,
//                             node->for_stmt.stmt_count,
//                             indent + 2);
//         } break;
//
//         case AST_BREAK_STMT:
//             printf("break");
//             break;
//
//         case AST_CONTINUE_STMT:
//             printf("continue");
//             break;
//
//         case AST_RETURN_STMT:
//             printf("return ");
//             print_node(node->return_stmt.expression, 0);
//             break;
//
//         case AST_EXPR_STMT:
//             print_node(node->expr_stmt.expression, 0);
//             break;
//
// /* ----------------------------------------
//         EXPRESSIONS
// ---------------------------------------- */
//
//         case AST_UNARY: {
//             printf("unary (%d) ", node->unary.op);
//             print_node(node->unary.operand, 0);
//         } break;
//
//         case AST_BINARY: {
//             printf("binary (%d) ", node->binary.op);
//             print_node(node->binary.left, 0);
//             printf(" , ");
//             print_node(node->binary.right, 0);
//         } break;
//
//         case AST_FUNCTION_CALL: {
//             print_slice(node->function_call.identifier);
//             printf("(");
//
//             for (usize i = 0; i < node->function_call.arg_count; i++) {
//                 print_node(node->function_call.arguments[i], 0);
//                 if (i + 1 < node->function_call.arg_count) printf(", ");
//             }
//
//             printf(")");
//         } break;
//
//         case AST_ARRAY_INDEX: {
//             print_node(node->array_index.array, 0);
//             printf("[");
//             print_node(node->array_index.index, 0);
//             printf("]");
//         } break;
//
//         case AST_MEMBER_ACCESS: {
//             print_node(node->member_access.object, 0);
//             printf(".");
//             print_slice(node->member_access.member);
//         } break;
//
//         case AST_IDENTIFIER:
//             print_slice(node->identifier.value);
//             break;
//
//         case AST_LITERAL:
//             print_slice(node->literal.value);
//             break;
//
// /* ----------------------------------------
//         PATTERNS
// ---------------------------------------- */
//
//         case AST_PATTERN_IDENT:
//             print_slice(node->pattern_ident.identifier);
//             break;
//
//         case AST_PATTERN_LITERAL:
//             print_slice(node->pattern_literal.literal.value);
//             break;
//
//         case AST_PATTERN_VARIANT: {
//             print_slice(node->pattern_variant.variant);
//             printf("(");
//
//             for (usize i = 0; i < node->pattern_variant.count; i++) {
//                 print_node(node->pattern_variant.patterns[i], 0);
//                 if (i + 1 < node->pattern_variant.count) printf(", ");
//             }
//
//             printf(")");
//         } break;
//
// /* ----------------------------------------
//         ERROR
// ---------------------------------------- */
//
//         case AST_ERROR:
//             printf("<error node>");
//             break;
//
//         default:
//             printf("unhandled ast node type");
//             break;
//     }
//
//     println();
//     print_indent(indent);
//     println("}\n");
// }
//
// /* ----------------------------------------
//         PROGRAM
// ---------------------------------------- */
//
// void print_program(Program* program) {
//     for (usize i = 0; i < program->count; i++) {
//         print_node(program->declarations[i], 0);
//     }
// }

#include <stdio.h>
#include <string.h>

static int indent_level = 0;

#define INDENT_SIZE 2

static void print_indent() {
    for (int i = 0; i < indent_level * INDENT_SIZE; i++) {
        putchar(' ');
    }
}

static void print_slice(AstSlice slice) {
    printf("%.*s", (int)slice.len, slice.ptr);
}

static void print_type(AstType* type) {
    if (!type) {
        printf("<null>");
        return;
    }

    switch (type->kind) {
        case TYPE_ERR:
            printf("<error type>");
            break;
        case TYPE_BASIC:
            print_slice(type->identifier);
            break;
        case TYPE_POINTER:
            printf("*");
            print_type(type->pointee);
            break;
        case TYPE_ARRAY:
            printf("[");
            if (type->array.size_expr) {
                printf("expr");
            }
            printf("]");
            print_type(type->array.element_type);
            break;
    }
}

static void print_node(AstNode* node);

static void print_module_decl(AstModuleDecl* decl) {
    printf("module ");
    for (usize i = 0; i < decl->count; i++) {
        if (i > 0) printf(".");
        print_slice(decl->segments[i]);
    }
    printf("\n");
}

static void print_import_decl(AstImportDecl* decl) {
    printf("import ");
    for (usize i = 0; i < decl->count; i++) {
        if (i > 0) printf(".");
        print_slice(decl->segments[i]);
    }
    printf("\n");
}

static void print_struct_decl(AstStructDecl* decl) {
    printf("struct ");
    print_slice(decl->identifier);
    printf(" {\n");
    
    indent_level++;
    for (usize i = 0; i < decl->count; i++) {
        print_indent();
        print_slice(decl->fields[i].identifier);
        printf(": ");
        print_type(decl->fields[i].type);
        printf("\n");
    }
    indent_level--;
    
    print_indent();
    printf("}\n");
}

static void print_enum_decl(AstEnumDecl* decl) {
    printf("enum ");
    print_slice(decl->identifier);
    printf(" {\n");
    
    indent_level++;
    for (usize i = 0; i < decl->count; i++) {
        print_indent();
        print_slice(decl->variants[i].identifier);
        if (decl->variants[i].count > 0) {
            printf("(");
            for (usize j = 0; j < decl->variants[i].count; j++) {
                if (j > 0) printf(", ");
                print_type(decl->variants[i].types[j]);
            }
            printf(")");
        }
        printf("\n");
    }
    indent_level--;
    
    print_indent();
    printf("}\n");
}

static void print_impl_decl(AstImplDecl* decl) {
    printf("impl ");
    print_slice(decl->target);
    printf(" {\n");
    
    indent_level++;
    for (usize i = 0; i < decl->fn_count; i++) {
        print_indent();
        print_node(decl->functions[i]);
    }
    indent_level--;
    
    print_indent();
    printf("}\n");
}

static void print_function_decl(AstFunctionDecl* decl) {
    printf("fn ");
    print_slice(decl->identifier);
    printf("(");
    
    for (usize i = 0; i < decl->param_count; i++) {
        if (i > 0) printf(", ");
        print_slice(decl->parameters[i].identifier);
        printf(": ");
        print_type(decl->parameters[i].type);
    }
    
    printf(")");
    if (decl->return_type) {
        printf(" -> ");
        print_type(decl->return_type);
    }
    printf(" {\n");
    
    indent_level++;
    for (usize i = 0; i < decl->stmt_count; i++) {
        print_indent();
        print_node(decl->statements[i]);
    }
    indent_level--;
    
    print_indent();
    printf("}\n");
}

static void print_const_decl(AstConstDecl* decl) {
    printf("const ");
    print_slice(decl->identifier);
    printf(": ");
    print_type(decl->type);
    printf(" = ");
    print_node(decl->value);
    printf("\n");
}

static void print_var_decl(AstVarDecl* decl) {
    printf("var ");
    print_slice(decl->identifier);
    if (decl->type) {
        printf(": ");
        print_type(decl->type);
    }
    if (decl->value) {
        printf(" = ");
        print_node(decl->value);
    }
    printf("\n");
}

static void print_static_decl(AstStaticDecl* decl) {
    printf("static ");
    print_slice(decl->identifier);
    printf(": ");
    print_type(decl->type);
    if (decl->value) {
        printf(" = ");
        print_node(decl->value);
    }
    printf("\n");
}

static void print_assignment(AstAssignment* assign) {
    print_node(assign->lvalue);
    printf(" = ");
    print_node(assign->rvalue);
    printf("\n");
}

static void print_if_stmt(AstIfStmt* stmt) {
    printf("if ");
    print_node(stmt->expression);
    printf(" {\n");
    
    indent_level++;
    for (usize i = 0; i < stmt->stmt_count; i++) {
        print_indent();
        print_node(stmt->statements[i]);
    }
    indent_level--;
    
    print_indent();
    printf("}");
    
    if (stmt->else_stmt) {
        printf(" else ");
        if (stmt->else_stmt->kind == AST_IF_STMT) {
            print_node(stmt->else_stmt);
        } else {
            printf("{\n");
            indent_level++;
            print_indent();
            print_node(stmt->else_stmt);
            indent_level--;
            print_indent();
            printf("}\n");
        }
    } else {
        printf("\n");
    }
}

static void print_match_stmt(AstMatchStmt* stmt) {
    printf("match ");
    print_node(stmt->expression);
    printf(" {\n");
    
    indent_level++;
    for (usize i = 0; i < stmt->arm_count; i++) {
        print_indent();
        print_node(stmt->arms[i].pattern);
        printf(" => {\n");
        
        indent_level++;
        for (usize j = 0; j < stmt->arms[i].stmt_count; j++) {
            print_indent();
            print_node(stmt->arms[i].statements[j]);
        }
        indent_level--;
        
        print_indent();
        printf("}\n");
    }
    
    if (stmt->stmt_count > 0) {
        print_indent();
        printf("_ => {\n");
        indent_level++;
        for (usize i = 0; i < stmt->stmt_count; i++) {
            print_indent();
            print_node(stmt->statements[i]);
        }
        indent_level--;
        print_indent();
        printf("}\n");
    }
    
    indent_level--;
    print_indent();
    printf("}\n");
}

static void print_loop_stmt(AstLoopStmt* stmt) {
    printf("loop {\n");
    
    indent_level++;
    for (usize i = 0; i < stmt->stmt_count; i++) {
        print_indent();
        print_node(stmt->statements[i]);
    }
    indent_level--;
    
    print_indent();
    printf("}\n");
}

static void print_while_stmt(AstWhileStmt* stmt) {
    printf("while ");
    print_node(stmt->cond);
    printf(" {\n");
    
    indent_level++;
    for (usize i = 0; i < stmt->stmt_count; i++) {
        print_indent();
        print_node(stmt->statements[i]);
    }
    indent_level--;
    
    print_indent();
    printf("}\n");
}

static void print_for_stmt(AstForStmt* stmt) {
    printf("for (");
    if (stmt->init) print_node(stmt->init);
    printf("; ");
    if (stmt->cond) print_node(stmt->cond);
    printf("; ");
    if (stmt->step) print_node(stmt->step);
    printf(") {\n");
    
    indent_level++;
    for (usize i = 0; i < stmt->stmt_count; i++) {
        print_indent();
        print_node(stmt->statements[i]);
    }
    indent_level--;
    
    print_indent();
    printf("}\n");
}

static void print_return_stmt(AstReturnStmt* stmt) {
    printf("return");
    if (stmt->expression) {
        printf(" ");
        print_node(stmt->expression);
    }
    printf("\n");
}

static void print_expr_stmt(AstExprStmt* stmt) {
    print_node(stmt->expression);
    printf("\n");
}

static void print_unary(AstUnary* unary) {
    printf("(");
    // Print operator based on TokenKind
    printf("unary_op ");
    print_node(unary->operand);
    printf(")");
}

static void print_binary(AstBinary* binary) {
    printf("(");
    print_node(binary->left);
    printf(" binary_op ");
    print_node(binary->right);
    printf(")");
}

static void print_function_call(AstFunctionCall* call) {
    print_slice(call->identifier);
    printf("(");
    for (usize i = 0; i < call->arg_count; i++) {
        if (i > 0) printf(", ");
        print_node(call->arguments[i]);
    }
    printf(")");
}

static void print_array_index(AstArrayIndex* index) {
    print_node(index->array);
    printf("[");
    print_node(index->index);
    printf("]");
}

static void print_member_access(AstMemberAccess* access) {
    print_node(access->object);
    printf(".");
    print_slice(access->member);
}

static void print_identifier(AstIdentifier* ident) {
    print_slice(ident->value);
}

static void print_literal(AstLiteral* lit) {
    print_slice(lit->value);
}

static void print_pattern_ident(AstPatternIdent* pattern) {
    print_slice(pattern->identifier);
}

static void print_pattern_literal(AstPatternLiteral* pattern) {
    print_slice(pattern->literal.value);
}

static void print_pattern_variant(AstPatternVariant* pattern) {
    print_slice(pattern->variant);
    if (pattern->count > 0) {
        printf("(");
        for (usize i = 0; i < pattern->count; i++) {
            if (i > 0) printf(", ");
            print_node(pattern->patterns[i]);
        }
        printf(")");
    }
}

static void print_node(AstNode* node) {
    if (!node) {
        printf("<null>");
        return;
    }

    switch (node->kind) {
        case AST_MODULE_DECL:
            print_module_decl(&node->module_decl);
            break;
        case AST_IMPORT_DECL:
            print_import_decl(&node->import_decl);
            break;
        case AST_STRUCT_DECL:
            print_struct_decl(&node->struct_decl);
            break;
        case AST_ENUM_DECL:
            print_enum_decl(&node->enum_decl);
            break;
        case AST_IMPL_DECL:
            print_impl_decl(&node->impl_decl);
            break;
        case AST_FUNCTION_DECL:
            print_function_decl(&node->function_decl);
            break;
        case AST_STATIC_DECL:
            print_static_decl(&node->static_decl);
            break;
        case AST_CONST_DECL:
            print_const_decl(&node->const_decl);
            break;
        case AST_VAR_DECL:
            print_var_decl(&node->var_decl);
            break;
        case AST_ASSIGNMENT:
            print_assignment(&node->assignment);
            break;
        case AST_IF_STMT:
            print_if_stmt(&node->if_stmt);
            break;
        case AST_MATCH_STMT:
            print_match_stmt(&node->match_stmt);
            break;
        case AST_LOOP_STMT:
            print_loop_stmt(&node->loop_stmt);
            break;
        case AST_WHILE_STMT:
            print_while_stmt(&node->while_stmt);
            break;
        case AST_FOR_STMT:
            print_for_stmt(&node->for_stmt);
            break;
        case AST_BREAK_STMT:
            printf("break\n");
            break;
        case AST_CONTINUE_STMT:
            printf("continue\n");
            break;
        case AST_RETURN_STMT:
            print_return_stmt(&node->return_stmt);
            break;
        case AST_EXPR_STMT:
            print_expr_stmt(&node->expr_stmt);
            break;
        case AST_UNARY:
            print_unary(&node->unary);
            break;
        case AST_BINARY:
            print_binary(&node->binary);
            break;
        case AST_FUNCTION_CALL:
            print_function_call(&node->function_call);
            break;
        case AST_ARRAY_INDEX:
            print_array_index(&node->array_index);
            break;
        case AST_MEMBER_ACCESS:
            print_member_access(&node->member_access);
            break;
        case AST_IDENTIFIER:
            print_identifier(&node->identifier);
            break;
        case AST_LITERAL:
            print_literal(&node->literal);
            break;
        case AST_PATTERN_IDENT:
            print_pattern_ident(&node->pattern_ident);
            break;
        case AST_PATTERN_LITERAL:
            print_pattern_literal(&node->pattern_literal);
            break;
        case AST_PATTERN_VARIANT:
            print_pattern_variant(&node->pattern_variant);
            break;
        case AST_PATTERN_WILDCARD:
            printf("_");
            break;
        case AST_ERROR:
            printf("<error node>");
            break;
    }
}

void print_program(Program* program) {
    if (!program) {
        printf("Program is NULL\n");
        return;
    }

    printf("=== Program AST ===\n\n");
    
    for (usize i = 0; i < program->count; i++) {
        print_node(program->declarations[i]);
        printf("\n");
    }
    
    printf("=== End of Program ===\n");
}
