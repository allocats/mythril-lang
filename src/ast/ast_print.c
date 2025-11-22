#include <stdio.h>

#include "ast.h"
#include "types.h"

void print_indent(int depth) {
    for (int i = 0; i < depth; i++) {
        printf("  ");
    }
}

void print_slice(char* ptr, usize len) {
    printf("%.*s", (int)len, ptr);
}

void print_function(AstFunction* func, int depth) {
    print_indent(depth);
    printf("Function: ");
    print_slice(func -> name_ptr, func -> name_len);
    printf("  ->  ");
    print_slice(func -> ret_ptr, func -> ret_len);
    printf("\n");
    
    if (func -> param_count > 0) {
        print_indent(depth + 1);
        printf("Parameters (%zu):\n", func -> param_count);
        for (usize i = 0; i < func -> param_count; i++) {
            print_node(func -> params[i], depth + 2);
        }
    }
    
    print_indent(depth + 1);
    printf("Body:\n");
    print_node(func -> block, depth + 2);
}

void print_block(AstBlock* block, int depth) {
    print_indent(depth);
    printf("Block (%zu statements):\n", block -> count);
    for (usize i = 0; i < block -> count; i++) {
        print_node(block -> statements[i], depth + 1);
    }
}

void print_var_decl(AstVarDecl* decl, int depth) {
    print_indent(depth);
    printf("VarDecl: ");
    print_slice(decl -> type_ptr, decl -> type_len);
    printf(" (Pointer depth = %d) ", decl -> pointer_depth);
    print_slice(decl -> name_ptr, decl -> name_len);
    
    if (decl -> initializer) {
        printf(" =\n");
        print_node(decl -> initializer, depth + 1);
    } else {
        printf("\n");
    }
}

void print_assignment(AstAssignment* assign, int depth) {
    print_indent(depth);
    printf("Assignment:\n");
    
    print_indent(depth + 1);
    printf("Target:\n");
    print_node(assign -> target, depth + 2);
    
    print_indent(depth + 1);
    printf("Value:\n");
    print_node(assign -> value, depth + 2);
}

void print_literal(AstLiteral* lit, int depth) {
    print_indent(depth);
    printf("Literal: ");
    
    switch (lit -> type) {
        case T_NUMBER:
            printf("%ld (int)\n", lit -> value.int_value);
            break;
        case T_STR_LITERAL:
            printf("\"");
            print_slice(lit -> value.string.ptr, lit -> value.string.len);
            printf("\" (string)\n");
            break;
        default:
            printf("(unknown type)\n");
    }
}

void print_identifier(AstIdentifier* ident, int depth) {
    print_indent(depth);
    printf("Identifier: ");
    print_slice(ident -> name_ptr, ident -> name_len);
    printf("\n");
}

void print_function_call(AstFnCall* call, int depth) {
    print_indent(depth);
    printf("FunctionCall: ");
    print_slice(call -> name_ptr, call -> name_len);
    printf("\n");
    
    if (call -> arg_count > 0) {
        print_indent(depth + 1);
        printf("Arguments (%zu):\n", call -> arg_count);
        for (usize i = 0; i < call -> arg_count; i++) {
            print_node(call -> args[i], depth + 2);
        }
    }
}

void print_return(AstReturn* ret, int depth) {
    print_indent(depth);
    printf("Return:\n");
    if (ret -> expression) {
        print_node(ret -> expression, depth + 1);
    }
}

void print_binary_expr(AstExpr* expr) {

}

void print_expr(AstExpr* expr, int depth) {
    print_indent(depth);
    printf("Expression:\n");

    if (expr -> kind == EXPR_BINARY) {
        print_binary_expr(expr);
    } else {
        printf("NOT SUPPORTED YET");
    }
}

void print_node(AstNode* node, int depth) {
    if (!node) {
        print_indent(depth);
        printf("(null node)\n");
        return;
    }
    
    switch (node -> type) {
        case A_FUNC:
            print_function(&node -> ast.function, depth);
            break;
        case A_BLOCK:
            print_block(&node -> ast.block, depth);
            break;
        case A_VAR_DECL:
            print_var_decl(&node -> ast.var_decl, depth);
            break;
        case A_ASSIGNMENT:
            print_assignment(&node -> ast.assignment, depth);
            break;
        case A_LITERAL:
            print_literal(&node -> ast.literal, depth);
            break;
        case A_IDENTIFIER:
            print_identifier(&node -> ast.identifier, depth);
            break;
        case A_FUNC_CALL:
            print_function_call(&node -> ast.function_call, depth);
            break;
        case A_RETURN:
            print_return(&node -> ast.ret, depth);
            break;
        case A_EXPR:
            print_expr(&node -> ast.expression, depth);
            break;
        default:
            print_indent(depth);
            printf("Unknown node type: %s\n", AST_TYPES_STRINGS[node -> type]);
    }
}

void print_program(Program* program) {
    printf("=== Program AST ===\n\n");
    printf("Total functions: %zu\n\n", program -> count);
    
    for (usize i = 0; i < program -> count; i++) {
        print_node(&program -> items[i], 0);
        printf("\n");
    }
    
    printf("=== End AST ===\n");
}
