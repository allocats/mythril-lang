#include "ast.h"
#include "types.h"

#include "../token/token.h"
// #include "../utils/macros.h"

#include <stdio.h>

#define DEFAULT_CAP 256

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

Program* ast_build(
    Tokens* tokens,
    ArenaAllocator* arena
) {
    Parser parser = {
        .arena = arena,
        .tokens = tokens,
        .count = tokens -> count,
        .index = 0
    };

    Program* program = arena_alloc(arena, sizeof(*program));

    program -> count = 0;
    program -> items = arena_alloc(arena, sizeof(AstNode) * DEFAULT_CAP);

    while (parser.index < parser.count) {
        AstNode* node = &(program -> items[program -> count++]);
        Token* tok = parser_peek(&parser);

        if (tok -> type == T_FUNCTION) {
            parse_function(node, &parser);
        }     
    }

    return program;
}

AstNode* parse_expression(Parser* p) {
    Token* token = parser_advance(p);

    AstNode* left = nullptr;

    if (token -> type == T_IDENTIFIER) {
        // function call
        if (parser_check(p, T_LEFTPAREN)) {
            return parse_function_call(p, token);
        }

        left = ast_create_identifier(
            p -> arena,
            token -> literal,
            token -> len
        );
    } else if (token -> type == T_STR_LITERAL) {
        left = ast_create_string_literal(
            p -> arena, 
            token -> literal + 1,
            token -> len - 2
        );
    } else if (token -> type == T_NUMBER) {
        u64 number = 0;

        for (u32 i = 0; i < token -> len; i++) {
            char c = token -> literal[i];
            u64 digit = (u64)(c - '0');
            number = number * 10 + digit;
        }

        left = ast_create_number_literal(
            p -> arena,
            number
        );
    } else {
        return nullptr;
    }


    Token* op = parser_peek(p);

    if (IS_EXPR_OP(op -> type)) {
        parser_advance(p);
        AstNode* right =  parse_expression(p);

        if (right != nullptr) {
            return ast_create_binary_expr(
                p -> arena,
                op -> type,
                left,
                right
            );
        }
    }

    return left;
}

AstNode* parse_statement(Parser* p) {
    Token* token = parser_peek(p);

    if (IS_PRIMITIVE_TYPE(token -> type)) {
        Token* type = parser_advance(p);

        u32 pointer_depth = 0;
        while (parser_check(p, T_ASTERIX)) {
            parser_advance(p);
            pointer_depth++;
        }

        Token* identifier = parser_expect(p, T_IDENTIFIER, "Expected identifier");

        if (parser_check(p, T_SEMICOLON)) {
            parser_advance(p);

            return ast_create_var_decl(
                p -> arena,
                type -> literal,
                type -> len,
                pointer_depth,
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
                pointer_depth,
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

    if (token -> type == T_VOID) {
        Token* type = parser_advance(p);

        u32 pointer_depth = 0;
        while (parser_check(p, T_ASTERIX)) {
            parser_advance(p);
            pointer_depth++;
        }

        if (pointer_depth == 0) {
            fprintf(stderr, "Error: Invalid type 'void' maybe you meant 'void*'?\n");
            exit(1);
        }

        Token* identifier = parser_expect(p, T_IDENTIFIER, "Expected identifier");

        if (parser_check(p, T_SEMICOLON)) {
            parser_advance(p);

            return ast_create_var_decl(
                p -> arena,
                type -> literal,
                type -> len,
                pointer_depth,
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
                pointer_depth,
                identifier -> literal,
                identifier -> len,
                expr
            );
        }
    }

    if (token -> type == T_RETURN) {
        parser_advance(p);
        AstNode* expr = parse_expression(p);
        parser_expect(p, T_SEMICOLON, "Expected ';'");
        return ast_create_return(p -> arena, expr);
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

            u32 param_pointer_depth = 0;
            while (parser_check(p, T_ASTERIX)) {
                parser_advance(p);
                param_pointer_depth++;
            }

            Token* param_name = parser_expect(p, T_IDENTIFIER, "Expected parameter name");

            params[param_count++] = ast_create_var_decl(
                p -> arena,
                param_type -> literal,
                param_type -> len,
                param_pointer_depth,
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

    if (parser_check(p, T_SEMICOLON)) {
        parser_advance(p);

        ast_create_func(
            func_node,
            fn_name -> literal,
            fn_name -> len,
            ret_type -> literal,
            ret_type -> len,
            params,
            param_count,
            nullptr
        );

        return;
    }

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
            } else if (!parser_check(p, T_RIGHTPAREN)) {
                fprintf(stderr, "Error: Expected ',' or ')' in function call\n");
                exit(1);
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
