#include <stdio.h>

#include "ast.h"
#include "types.h"

#define println(fmt, ...) \
    fprintf(stdout, fmt "\n", ##__VA_ARGS__);

void print_indent(u32 indent) {
    for (u32 i = 0; i < indent; i++) {
        printf("    ");
    }
}

void print_slice(AstSlice slice) {
    printf("%.*s", (u32) slice.len, slice.ptr);
}

void print_node(AstNode* node, u32 indent) {
    if (!node || node == nullptr) {
        print_indent(indent);
        println("<null node>");
        return;
    }

    print_indent(indent);
    
    println("[Node: %s] {", AST_KIND_STRINGS[node -> kind]);

    print_indent(indent + 1);

    switch (node -> kind) {
        case AST_MODULE_DECL: {
            printf("module: ");
            
            for (usize i = 0; i < node -> module_decl.count; i++) {
                print_slice(node -> module_decl.segments[i]);

                if (i + 1 < node -> module_decl.count) {
                    printf("::");
                }
            }
        } break;

        case AST_IMPORT_DECL: {
            printf("import: ");
            
            for (usize i = 0; i < node -> import_decl.count; i++) {
                print_slice(node -> import_decl.segments[i]);

                if (i + 1 < node -> import_decl.count) {
                    printf("::");
                }
            }
        } break;

        default: {
            println("unhandled ast node type");
        } break;
    }

    println();
    print_indent(indent);
    println("}\n");
}

void print_program(Program* program) {
    for (usize i = 0; i < program -> count; i++) {
        print_node(program -> declarations[i], 0);
    }
}
