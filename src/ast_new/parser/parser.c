#include "parser.h"

#include "../ast.h"
#include "../../token/token.h"
#include "../../utils/macros.h"

[[gnu::always_inline]]
Token* parser_peek(Parser *p) {
    MEOW_ASSERT(p -> index < p -> count, "Index out of bounds in parser_peek");
    return &p -> tokens[p -> index];
}

[[gnu::always_inline]]
Token* parser_peek_ahead(Parser *p) {
    MEOW_ASSERT(p -> index < p -> count - 1, "Index out of bounds in parser_peek");
    return &p -> tokens[p -> index + 1];
}

[[gnu::always_inline]]
Token* parser_advance(Parser *p) {
    MEOW_ASSERT(p -> index < p -> count, "Index out of bounds in parser_advance");
    return &p -> tokens[p -> index++];
}

[[gnu::gnu_inline]] inline
Token* parser_advance_expect(Parser *p, TokenType type, const char* msg) {
    MEOW_ASSERT(p -> index < p -> count, "Index out of bounds in parser_advance");

    Token* token = &p -> tokens[p -> index++];
    if (token -> type != type) {
        ast_error(token, p -> buffer, msg);
    }
    return token;
}

[[gnu::always_inline]]
b32 parser_check(Parser *p, TokenType type) {
    Token* tok = parser_peek(p);
    return tok -> type == type;
}

[[gnu::always_inline]]
b32 parser_check_ahead(Parser *p, TokenType type) {
    Token* tok = parser_peek_ahead(p);
    return tok -> type == type;
}



AstNode* parse_function_decl(Parser *p) {
    AstNode* fn_node = arena_alloc(p -> arena, sizeof(*fn_node));

    fn_node -> kind = AST_FUNCTION;

    fn_node -> function.params.nodes = arena_alloc(p -> arena, sizeof(AstNode) * 8);
    fn_node -> function.params.count = 0;
    fn_node -> function.params.cap   = 8;

    parser_advance(p); // skip inital token

    Token* return_type = parser_advance(p);
    if (!IS_PRIMITIVE_TYPE(return_type -> type) && return_type -> type != T_IDENTIFIER) {
        ast_error(return_type, p -> buffer, "Expected a return type");
    }

    Token* func_name = parser_advance(p);
    if (func_name -> type != T_IDENTIFIER) {
        ast_error(func_name, p -> buffer, "Expected function name");
    }

    if (!parser_check(p, T_LEFTPAREN)) {
        ast_error(&p -> tokens[p -> index], p -> buffer, "Expected '('");
    }

    while (!parser_check(p, T_RIGHTPAREN)) {
        parse_vec_var_decl(
            p,
            &fn_node -> function.params
        );

        if (!parser_check(p, T_COMMA) && !parser_check(p, T_RIGHTPAREN)) {
            ast_error(parser_peek(p), p -> buffer, "Expected ',' or ')'");
        }

        if (!parser_check(p, T_COMMA) && !parser_check_ahead(p, T_RIGHTPAREN)) {
            ast_error(parser_peek(p), p -> buffer, "Expected ',' between parameters");
        }
    }

    parser_advance_expect(p, T_RIGHTPAREN, "Expected ')'");

    if (!parser_check(p, T_LEFTBRACE)) {
        ast_error(parser_peek(p), p -> buffer, "Expected '{' for start of function block");
    }

    fn_node -> function.return_type = ast_create_identifier(
        p -> arena,
        return_type -> literal,
        return_type -> len
    ); 

    fn_node -> function.identifier = ast_create_identifier(
        p -> arena,
        func_name -> literal,
        func_name -> len
    ); 

    fn_node -> function.block = parse_block(p);

    return fn_node;
}

AstVec parse_block(Parser *p) {
    parser_advance(p); // skip initial token "{"

    AstVec vec = {
        .nodes = arena_alloc(p -> arena, sizeof(AstNode) * 8),
        .count = 0,
        .cap = 8
    };

    while (!parser_check(p, T_RIGHTBRACE)) {
    }

    return vec;
}

AstNode* parse_var_decl(Parser *p) {
    ArenaAllocator* arena = p -> arena;

    AstNode* node = arena_alloc(arena, sizeof(*node));

    Token* type_tok = parser_advance(p);

    if (type_tok -> type != T_IDENTIFIER && !IS_PRIMITIVE_TYPE(type_tok -> type)) {
        ast_error(type_tok, p -> buffer, "Expected type");
    }

    Token* ident_tok = parser_advance_expect(p, T_IDENTIFIER, "Expected identifier");

    node -> kind = AST_VAR_DECL;
    
    node -> var_decl.type = ast_create_identifier(arena, type_tok -> literal, type_tok -> len);
    node -> var_decl.identifier = ast_create_identifier(arena, ident_tok -> literal, ident_tok -> len);
    node -> var_decl.value = nullptr;

    return node;
}

void parse_vec_var_decl(Parser *p, AstVec* vec) {
    ArenaAllocator* arena = p -> arena;

    Token* type_tok = parser_advance(p);

    if (type_tok -> type != T_IDENTIFIER && !IS_PRIMITIVE_TYPE(type_tok -> type)) {
        ast_error(type_tok, p -> buffer, "Expected type");
    }

    Token* ident_tok = parser_advance_expect(p, T_IDENTIFIER, "Expected identifier");

    if (vec -> count >= vec -> cap) {
        usize new_cap = vec -> cap == 0 ? 8 : vec -> cap * 2;

        if (MEOW_UNLIKELY(new_cap > 128)) {
            ast_error(ident_tok, p -> buffer, "wtf are you doing? more than 128 parameters");
        }

        vec -> nodes = arena_realloc(arena, vec -> nodes, vec -> cap, new_cap);
        vec -> cap = new_cap;
    }

    AstNode* node = &vec -> nodes[vec -> count++];

    node -> kind = AST_VAR_DECL;
    
    node -> var_decl.type = ast_create_identifier(arena, type_tok -> literal, type_tok -> len);
    node -> var_decl.identifier = ast_create_identifier(arena, ident_tok -> literal, ident_tok -> len);
    node -> var_decl.value = nullptr;
}
