#include "ast.h"
#include "types.h"

#include "../token/token.h"
#include "../utils/macros.h"

#include <stdio.h>

#define DEFAULT_CAP 256

AstNode* ast_create_func(
    AstNode* node,
    char* name_ptr,
    usize name_len,
    char* ret_ptr,
    usize ret_len,
    AstNode** params,
    usize param_count,
    AstNode* block
);

AstNode* ast_create_block(
    ArenaAllocator* arena,
    AstNode** statements,
    usize count
);

AstNode* ast_create_func_call(
    ArenaAllocator* arena,
    char* name_ptr,
    usize name_len,
    AstNode** args,
    usize arg_count
);

AstNode* ast_create_statement(
    Parser* p
);

AstNode* ast_create_var_decl(
    ArenaAllocator* arena,
    char* type_ptr,
    usize type_len,
    char* name_ptr,
    usize name_len,
    AstNode* initializer 
);

AstNode* ast_create_assignment(
    ArenaAllocator* arena,
    AstNode* target,
    AstNode* value 
);

AstNode* ast_create_number_literal(
    ArenaAllocator* arena,
    u64 literal
);

AstNode* ast_create_string_literal(
    ArenaAllocator* arena,
    char* ptr,
    u32 len
);

AstNode* ast_create_identifier(
    ArenaAllocator* arena,
    char* name_ptr,
    usize name_len
);

AstNode* ast_create_return(
    ArenaAllocator* arena,
    AstNode* expr
);

Token* parser_peek(Parser* p) {
    if (p -> index >= p -> count) return nullptr;
    return &(p -> tokens -> items[p -> index]);
}

Token* parser_advance(Parser* p) {
    if (p -> index >= p -> count) return nullptr;
    return &(p -> tokens -> items[p -> index++]);
}

b32 parser_check(Parser* p, TokenType type) {
    Token* tok = parser_peek(p);
    return tok && tok -> type == type;
}

Token* parser_expect(Parser* p, TokenType type, const char* err_msg) {
    if (p -> index >= p -> count) return nullptr;

    Token* tok = &(p -> tokens -> items[p -> index]);
    if (tok -> type != type) {
        fprintf(stderr, "%s\n", err_msg);
        exit(1);
    }

    p -> index++;
    return tok;
}

void parse_function(
    AstNode* func_node,
    Parser* p
);

AstNode* parse_function_call(
    Parser* p,
    Token* name_token
);

ProgramFunctions* ast_build(
    Tokens* tokens,
    ArenaAllocator* arena
) {
    Parser parser = {
        .arena = arena,
        .tokens = tokens,
        .count = tokens -> count,
        .index = 0
    };

    ProgramFunctions* funcs = arena_alloc(arena, sizeof(*funcs));

    funcs -> count = 0;
    funcs -> items = arena_alloc(arena, sizeof(AstNode) * DEFAULT_CAP);

    while (parser.index < parser.count) {
        AstNode* func = &(funcs -> items[funcs -> count++]);
        Token* tok = parser_peek(&parser);

        if (tok -> type == T_FUNCTION) {
            parse_function(func, &parser);
        }
    }

    return funcs;
}

AstNode* parse_expression(Parser* p) {
    Token* token = parser_advance(p);

    if (token -> type == T_IDENTIFIER) {
        // function call
        if (parser_check(p, T_LEFTPAREN)) {
            return parse_function_call(p, token);
        }

        return ast_create_identifier(
            p -> arena,
            token -> literal,
            token -> len
        );
    }

    // TODO: EVERYTHING
    if (token -> type == T_STR_LITERAL) {
        return ast_create_string_literal(
            p -> arena, 
            token -> literal + 1,
            token -> len - 2
        );
    }

    if (token -> type == T_NUMBER) {
        u64 number = 0;

        for (u32 i = 0; i < token -> len; i++) {
            char c = token -> literal[i];
            u64 digit = (u64)(c - '0');
            number = number * 10 + digit;
        }

        return ast_create_number_literal(
            p -> arena,
            number
        );
    }

    return nullptr;
}

AstNode* parse_statement(Parser* p) {
    Token* token = parser_peek(p);

    if (token -> type == T_RETURN) {
        parser_advance(p);
        AstNode* expr = parse_expression(p);
        parser_expect(p, T_SEMICOLON, "Expected ';'");
        return ast_create_return(p -> arena, expr);
    }

    if (IS_PRIMITIVE_TYPE(token -> type)) {
        Token* type = parser_advance(p);
        Token* identifier = parser_expect(p, T_IDENTIFIER, "Expected identifier");

        if (parser_check(p, T_SEMICOLON)) {
            parser_advance(p);

            return ast_create_var_decl(
                p -> arena,
                type -> literal,
                type -> len,
                identifier -> literal,
                identifier -> len,
                NULL
            );
        } else if (parser_check(p, T_EQUALS)) {
            parser_advance(p);

            AstNode* expr = parse_expression(p);
            parser_expect(p, T_SEMICOLON, "Expected ';'");

            return ast_create_var_decl(
                p -> arena,
                token -> literal,
                token -> len,
                identifier -> literal,
                identifier -> len,
                expr
            );
        }
    }

    if (token -> type == T_IDENTIFIER) {
        Token* identifier = parser_advance(p);
        
        if (parser_check(p, T_EQUALS)) {
            AstNode* value = parse_expression(p);

            parser_expect(p, T_SEMICOLON, "Expected ';'");

            return ast_create_assignment(
                p -> arena,
                ast_create_identifier(
                    p -> arena,
                    token -> literal,
                    token -> len
                ),
                value
            );
        }

        AstNode* expr = ast_create_identifier(
            p -> arena,
            identifier -> literal,
            identifier -> len
        );

        if (parser_check(p, T_LEFTPAREN)) {
            expr  = parse_function_call(p, identifier);
        }

        parser_expect(p, T_SEMICOLON, "Expected ';'");
        return expr;
    }

    return nullptr;
}

AstNode* parse_block(Parser* p) {
    parser_expect(p, T_LEFTBRACE, "Expected '{' for start of function block");

    AstNode** statements = arena_alloc(p -> arena, sizeof(*statements) * DEFAULT_CAP);
    usize statement_count = 0;

    if (!parser_check(p, T_RIGHTBRACE)) {
        while (!parser_check(p, T_RIGHTBRACE)) {
            statements[statement_count++] = parse_statement(p);
        }
    }

    parser_expect(p, T_RIGHTBRACE, "Expected '}' for end of function block");

    return ast_create_block(
        p -> arena,
        statements,
        statement_count
    );
}

void parse_function(
    AstNode* func_node,
    Parser* p
) {
    parser_advance(p);

    Token* ret_type = parser_advance(p);
    Token* fn_name = parser_expect(p, T_IDENTIFIER, "Expected function name");

    parser_expect(p, T_LEFTPAREN, "Expected '('");

    AstNode** params = arena_alloc(p -> arena, sizeof(*params) * DEFAULT_CAP);
    usize param_count = 0;

    if (!parser_check(p, T_RIGHTPAREN)) {
        while (!parser_check(p, T_RIGHTPAREN)){
            Token* param_type = parser_advance(p);
            Token* param_name = parser_expect(p, T_IDENTIFIER, "Expected parameter name");

            params[param_count++] = ast_create_var_decl(
                p -> arena,
                param_type -> literal,
                param_type -> len,
                param_name -> literal,
                param_name -> len,
                NULL
            );

            if (parser_check(p, T_COMMA)) {
                parser_advance(p);
            }
        } 
    }
    
    parser_expect(p, T_RIGHTPAREN, "Expected ')'");

    AstNode* block = parse_block(p);

    ast_create_func(
        func_node,
        fn_name -> literal,
        fn_name -> len,
        ret_type -> literal,
        ret_type -> len,
        params,
        param_count,
        block
    );
}

AstNode* parse_function_call(
    Parser* p,
    Token* name_token
) {
    parser_expect(p, T_LEFTPAREN, "Expected '('");

    AstNode** args = arena_alloc(p -> arena, sizeof(*args) * DEFAULT_CAP);
    usize arg_count = 0;

    if (!parser_check(p, T_RIGHTPAREN)) {
        while (!parser_check(p, T_RIGHTPAREN)) { 
            args[arg_count++] = parse_expression(p);
            if (parser_check(p, T_COMMA)) {
                parser_advance(p);
            }
        } 
    }

    parser_expect(p, T_RIGHTPAREN, "Expected ')'");

    return ast_create_func_call(
        p -> arena,
        name_token -> literal,
        name_token -> len,
        args,
        arg_count
    );
}

AstNode* ast_create_func(
    AstNode* node,
    char* name_ptr,
    usize name_len,
    char* ret_ptr,
    usize ret_len,
    AstNode** params,
    usize param_count,
    AstNode* block
) {
    node -> type = A_FUNC;

    node -> as.function.name_ptr = name_ptr; 
    node -> as.function.name_len = name_len;

    node -> as.function.ret_ptr = ret_ptr;
    node -> as.function.ret_len = ret_len;

    node -> as.function.params = params;
    node -> as.function.param_count = param_count;

    node -> as.function.block = block;

    return node;
}

AstNode* ast_create_var_decl(
    ArenaAllocator* arena,
    char* type_ptr,
    usize type_len,
    char* name_ptr,
    usize name_len,
    AstNode* initializer 
) {
    AstNode* node = arena_alloc(arena, sizeof(*node));

    node -> type = A_VAR_DECL;

    node -> as.var_decl.type_ptr = type_ptr;
    node -> as.var_decl.type_len = type_len;

    node -> as.var_decl.name_ptr = name_ptr;
    node -> as.var_decl.name_len = name_len;

    node -> as.var_decl.initializer = initializer;

    return node;
}

AstNode* ast_create_func_call(
    ArenaAllocator* arena,
    char* name_ptr,
    usize name_len,
    AstNode** args,
    usize arg_count
) {
    AstNode* node = arena_alloc(arena, sizeof(*node));

    node -> type = A_FUNC_CALL;

    node -> as.function_call.name_ptr = name_ptr;
    node -> as.function_call.name_len = name_len;

    node -> as.function_call.args = args;
    node -> as.function_call.arg_count = arg_count;

    return node;
}

AstNode* ast_create_block(
    ArenaAllocator* arena,
    AstNode** statements,
    usize count
) {
    AstNode* node = arena_alloc(arena, sizeof(*node));

    node -> type = A_BLOCK;

    node -> as.block.statements = statements;
    node -> as.block.count = count;

    return node;
}

AstNode* ast_create_identifier(
    ArenaAllocator* arena,
    char* name_ptr,
    usize name_len
) {
    AstNode* node = arena_alloc(arena, sizeof(*node));

    node -> type = A_IDENT;

    node -> as.identifier.name_ptr = name_ptr;
    node -> as.identifier.name_len = name_len;

    return node;
}

AstNode* ast_create_number_literal(
    ArenaAllocator* arena,
    u64 value 
) {
    AstNode* node = arena_alloc(arena, sizeof(*node));

    node -> type = A_LITERAL;

    node -> as.literal.type = T_NUMBER;
    node -> as.literal.value.int_value = value;

    return node;
}

AstNode* ast_create_string_literal(
    ArenaAllocator* arena,
    char* ptr,
    u32 len
) {
    AstNode* node = arena_alloc(arena, sizeof(*node));

    node -> type = A_LITERAL;

    node -> as.literal.type = T_STR_LITERAL;
    node -> as.literal.value.string.ptr = ptr;
    node -> as.literal.value.string.len = len;

    return node;
}

AstNode* ast_create_return(
    ArenaAllocator* arena,
    AstNode* expr
) {
    AstNode* node = arena_alloc(arena, sizeof(*node));

    node -> type = A_RETURN;
    node -> as.ret.expression = expr;

    return node;
}

AstNode* ast_create_assignment(
    ArenaAllocator* arena,
    AstNode* target,
    AstNode* value 
) {
    AstNode* node = arena_alloc(arena, sizeof(*node));

    node -> type = A_ASSIGNMENT;

    node -> as.assignment.target = target;
    node -> as.assignment.value = value;

    return node;
}

// Helper to print indentation
static void print_indent(int depth) {
    for (int i = 0; i < depth; i++) {
        printf("  ");
    }
}

// Helper to print string slices
static void print_slice(char* ptr, usize len) {
    printf("%.*s", (int)len, ptr);
}

static void print_node(AstNode* node, int depth);

static void print_function(AstFunction* func, int depth) {
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

static void print_block(AstBlock* block, int depth) {
    print_indent(depth);
    printf("Block (%zu statements):\n", block -> count);
    for (usize i = 0; i < block -> count; i++) {
        print_node(block -> statements[i], depth + 1);
    }
}

static void print_var_decl(AstVarDecl* decl, int depth) {
    print_indent(depth);
    printf("VarDecl: ");
    print_slice(decl -> type_ptr, decl -> type_len);
    printf(" ");
    print_slice(decl -> name_ptr, decl -> name_len);
    
    if (decl -> initializer) {
        printf(" =\n");
        print_node(decl -> initializer, depth + 1);
    } else {
        printf("\n");
    }
}

static void print_assignment(AstAssignment* assign, int depth) {
    print_indent(depth);
    printf("Assignment:\n");
    
    print_indent(depth + 1);
    printf("Target:\n");
    print_node(assign -> target, depth + 2);
    
    print_indent(depth + 1);
    printf("Value:\n");
    print_node(assign -> value, depth + 2);
}

static void print_literal(AstLiteral* lit, int depth) {
    print_indent(depth);
    printf("Literal: ");
    
    switch (lit -> type) {
        case T_NUMBER:
            printf("%d (int)\n", lit -> value.int_value);
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

static void print_identifier(ASTIdentifier* ident, int depth) {
    print_indent(depth);
    printf("Identifier: ");
    print_slice(ident -> name_ptr, ident -> name_len);
    printf("\n");
}

static void print_function_call(AstFnCall* call, int depth) {
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

static void print_return(AstReturn* ret, int depth) {
    print_indent(depth);
    printf("Return:\n");
    if (ret -> expression) {
        print_node(ret -> expression, depth + 1);
    }
}

static void print_node(AstNode* node, int depth) {
    if (!node) {
        print_indent(depth);
        printf("(null node)\n");
        return;
    }
    
    switch (node -> type) {
        case A_FUNC:
            print_function(&node -> as.function, depth);
            break;
        case A_BLOCK:
            print_block(&node -> as.block, depth);
            break;
        case A_VAR_DECL:
            print_var_decl(&node -> as.var_decl, depth);
            break;
        case A_ASSIGNMENT:
            print_assignment(&node -> as.assignment, depth);
            break;
        case A_LITERAL:
            print_literal(&node -> as.literal, depth);
            break;
        case A_IDENT:
            print_identifier(&node -> as.identifier, depth);
            break;
        case A_FUNC_CALL:
            print_function_call(&node -> as.function_call, depth);
            break;
        case A_RETURN:
            print_return(&node -> as.ret, depth);
            break;
        default:
            print_indent(depth);
            printf("Unknown node type: %s\n", AST_TYPES_STRINGS[node -> type]);
    }
}

void print_program(ProgramFunctions* funcs) {
    printf("=== Program AST ===\n\n");
    printf("Total functions: %zu\n\n", funcs -> count);
    
    for (usize i = 0; i < funcs -> count; i++) {
        print_node(&funcs -> items[i], 0);
        printf("\n");
    }
    
    printf("=== End AST ===\n");
}
