#include "parser.h"
#include "types.h"

#include "../ast.h"
#include "../../hash/hash.h"
#include "../../token/token.h"
#include "../../utils/macros.h"
#include "../../utils/types.h"

[[gnu::always_inline]]
Token* parser_peek(Parser* p) {
    MEOW_ASSERT(p -> index < p -> count, "Index out of bounds in parser_peek");
    return &p -> tokens[p -> index];
}

[[gnu::always_inline]]
Token* parser_peek_ahead(Parser* p) {
    MEOW_ASSERT(p -> index < p -> count - 1, "Index out of bounds in parser_peek");
    return &p -> tokens[p -> index + 1];
}

[[gnu::always_inline]]
Token* parser_advance(Parser* p) {
    MEOW_ASSERT(p -> index < p -> count, "Index out of bounds in parser_advance");
    return &p -> tokens[p -> index++];
}

[[gnu::gnu_inline]] inline
Token* parser_advance_expect(Parser* p, TokenType type, const char* msg) {
    MEOW_ASSERT(p -> index < p -> count, "Index out of bounds in parser_advance");

    Token* token = &p -> tokens[p -> index++];
    if (token -> type != type) {
        ast_error(token, p, msg);
    }
    return token;
}

[[gnu::always_inline]]
b32 parser_check(Parser* p, TokenType type) {
    Token* tok = parser_peek(p);
    return tok -> type == type;
}

[[gnu::always_inline]]
b32 parser_check_ahead(Parser* p, TokenType type) {
    Token* tok = parser_peek_ahead(p);
    return tok -> type == type;
}

[[gnu::always_inline]]
b32 parser_check_advance(Parser* p, TokenType type) {
    Token* tok = parser_advance(p);
    return tok -> type == type;
}


AstNode* parse_function_decl(Parser* p) {
    AstNode* fn_node = arena_alloc(p -> arena, sizeof(*fn_node));

    fn_node -> kind = AST_FUNCTION;

    fn_node -> function.params.nodes = arena_alloc(p -> arena, sizeof(AstNode*) * 8);
    fn_node -> function.params.count = 0;
    fn_node -> function.params.cap   = 8;

    parser_advance(p); // skip inital token

    Token* func_name = parser_advance(p);
    if (func_name -> type != T_IDENTIFIER) {
        ast_error(func_name, p, "Expected function name");
    }

    parser_advance_expect(p, T_LEFTPAREN, "Expected '('");

    AstVec* params_vec = &fn_node -> function.params;

    while (!parser_check(p, T_RIGHTPAREN)) {
        AstNode* parameter = parse_var_decl(p);
        ast_vec_push(p -> arena, params_vec, parameter);

        if (parser_check(p, T_RIGHTPAREN)) {
            break;
        } 

        parser_advance_expect(p, T_COMMA, "Expected ',' between parameters");
    }

    parser_advance_expect(p, T_RIGHTPAREN, "Expected ')' after parameters");
    parser_advance_expect(p, T_COLON, "Expected ':' after parameters, before return type");

    Token* return_type = parser_advance(p);
    TokenType type = return_type -> type;

    if (!IS_PRIMITIVE_TYPE(type) && type != T_IDENTIFIER && type != T_VOID) {
        ast_error(return_type, p, "Expected a return type");
    }

    if (!parser_check(p, T_LEFTBRACE)) {
        ast_error(parser_peek(p), p, "Expected '{' for start of function block");
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

AstVec parse_block(Parser* p) {
    parser_advance(p); // skip initial token "{"

    AstVec vec = {
        .nodes = arena_alloc(p -> arena, sizeof(AstNode*) * 8),
        .count = 0,
        .cap = 8
    };

    while (!parser_check(p, T_RIGHTBRACE)) {
        AstNode* statement = parse_statement(p);
        ast_vec_push(p -> arena, &vec, statement);
    }

    if (vec.count == 0) {
        ast_warn(parser_peek(p), p, "Empty block");
    }

    parser_advance_expect(p, T_RIGHTBRACE, "Expected '}'");

    return vec;
}

AstNode* parse_statement(Parser* p) {
    Token* start = parser_peek(p);

    AstNode* statement = nullptr;

    b32 needs_semicolon = true;

    switch (start -> type) {
        case T_LET: {
            statement = parse_var_decl(p);
        } break; 
        
        case T_CONST: {
            statement = parse_const_decl(p);
        } break;

        case T_TRUE:
        case T_FALSE:
        case T_NULL:
        case T_NULLPTR:
        case T_NUMBER:
        case T_STR_LITERAL:
        case T_IDENTIFIER: {
            statement = parse_expr(p);
        } break;

        case T_WHILE: {
            needs_semicolon = false;
        } break;

        case T_FOR: {
            needs_semicolon = false;
        } break;

        case T_RETURN: {
            statement = parse_return(p);
        } break;

        default: {
            // parser_advance_expect(p, T_SEMICOLON, "Expected ';' after statement");
        } break;
    }

    if (needs_semicolon && statement != nullptr) {
        parser_advance_expect(p, T_SEMICOLON, "Expected ';' after statement");
    }

    return statement;
}

AstNode* parse_var_decl(Parser* p) {
    ArenaAllocator* arena = p -> arena;

    AstNode* node = arena_alloc(arena, sizeof(*node));

    Token* ident_tok = parser_advance_expect(p, T_IDENTIFIER, "Expected identifier");

    parser_advance_expect(p, T_COLON, "Expected ':' between identifier and type");

    Token* type_tok = parser_advance(p);

    if (type_tok -> type != T_IDENTIFIER && !IS_PRIMITIVE_TYPE(type_tok -> type)) {
        ast_error(type_tok, p, "Expected type");
    }

    usize pointer_depth = 0;

    while (parser_check(p, T_ASTERIX)) {
        parser_advance(p);
        pointer_depth++;
    }

    node -> kind = AST_VAR_DECL;
    
    node -> var_decl.type = ast_create_identifier(arena, type_tok -> literal, type_tok -> len);
    node -> var_decl.pointer_depth = pointer_depth;
    node -> var_decl.identifier = ast_create_identifier(arena, ident_tok -> literal, ident_tok -> len);
    node -> var_decl.value = parser_check(p, T_EQUALS) == false ? nullptr : parse_expr(p);

    return node;
}

AstNode* parse_const_decl(Parser* p) {
    ArenaAllocator* arena = p -> arena;

    AstNode* node = arena_alloc(arena, sizeof(*node));

    Token* ident_tok = parser_advance_expect(p, T_IDENTIFIER, "Expected identifier");

    parser_advance_expect(p, T_COLON, "Expected ':' between identifier and type");

    Token* type_tok = parser_advance(p);

    if (type_tok -> type != T_IDENTIFIER && !IS_PRIMITIVE_TYPE(type_tok -> type)) {
        ast_error(type_tok, p, "Expected type");
    }

    usize pointer_depth = 0;

    while (parser_check(p, T_ASTERIX)) {
        parser_advance(p);
        pointer_depth++;
    }

    parser_advance_expect(p, T_EQUALS, "Expected '=' const values must be intialised!");

    node -> kind = AST_CONST_DECL;
    
    node -> var_decl.type = ast_create_identifier(arena, type_tok -> literal, type_tok -> len);
    node -> var_decl.pointer_depth = pointer_depth;
    node -> var_decl.identifier = ast_create_identifier(arena, ident_tok -> literal, ident_tok -> len);
    node -> var_decl.value = parse_expr(p);

    return node;
}

AstNode* parse_expr(Parser* p) {
    return parse_expr_precedence(p, 0);
}

AstNode* parse_expr_precedence(Parser* p, u32 min_prec) {
    AstNode* left = parse_primary(p);

    while (is_binary_operator(parser_peek(p) -> type)) {
        TokenType op_type = parser_peek(p) -> type;
        u8 prec = get_precedence(op_type);

        if (prec < min_prec) {
            break;
        }

        Token* op = parser_advance(p);

        u8 next_prec = prec + (is_right_associative(op_type) ? 0 : 1);

        AstNode* right = parse_expr_precedence(p, next_prec);

        AstNode* bin_op = arena_alloc(p -> arena, sizeof(*bin_op));

        bin_op -> kind = AST_BINARY;
        bin_op -> binary.left = left;
        bin_op -> binary.op = op -> type;
        bin_op -> binary.right = right;

        left = bin_op;
    }

    return left;
}

AstNode* parse_primary(Parser* p) {
    Token current = *parser_peek(p);

    if (is_prefix_operator(current.type)) {
        Token op = *parser_advance(p);

        AstNode* operand = parse_expr_precedence(p, UNARY_PRECEDENCE);

        AstNode* unary_op = arena_alloc(p -> arena, sizeof(*unary_op));

        unary_op -> kind = AST_UNARY;

        unary_op -> unary.op = op.type;
        unary_op -> unary.operand = operand;

        return parse_postfix(p, unary_op);
    }

    if (current.type == T_NUMBER) {
        Token token = *parser_advance(p);
        
        AstNode* node = arena_alloc(p -> arena, sizeof(*node));

        node -> kind = AST_LITERAL;

        node -> literal.kind = LIT_INT;

        node -> literal.ptr  = token.literal;
        node -> literal.len  = token.len;

        return parse_postfix(p, node);
    }

    if (current.type == T_STR_LITERAL) {
        Token token = *parser_advance(p);
        
        AstNode* node = arena_alloc(p -> arena, sizeof(*node));

        node -> kind = AST_LITERAL;

        node -> literal.kind = LIT_STRING;

        node -> literal.ptr  = token.literal;
        node -> literal.len  = token.len;

        return parse_postfix(p, node);
    }

    if (current.type == T_TRUE || current.type == T_FALSE) {
        Token token = *parser_advance(p);
        
        AstNode* node = arena_alloc(p -> arena, sizeof(*node));

        node -> kind = AST_LITERAL;

        node -> literal.kind = LIT_BOOL;

        node -> literal.ptr  = token.literal;
        node -> literal.len  = token.len;

        return parse_postfix(p, node);
    }

    if (current.type == T_NULL || current.type == T_NULLPTR) {
        Token token = *parser_advance(p);
        
        AstNode* node = arena_alloc(p -> arena, sizeof(*node));

        node -> kind = AST_LITERAL;

        node -> literal.kind = LIT_NULL;

        node -> literal.ptr  = token.literal;
        node -> literal.len  = token.len;

        return parse_postfix(p, node);
    }

    if (current.type == T_IDENTIFIER) {
        Token token = *parser_advance(p);
        
        AstNode* node = arena_alloc(p -> arena, sizeof(*node));

        node -> kind = AST_IDENTIFIER;

        node -> identifier.ptr  = token.literal;
        node -> identifier.len  = token.len;
        node -> identifier.hash = hash_fnv1a(token.literal, token.len);

        return parse_postfix(p, node);
    }

    if (current.type == T_LEFTPAREN) {
        parser_advance(p);

        AstNode* expr = parse_expr(p);

        parser_advance_expect(p, T_RIGHTPAREN, "Expected ')' after expressoin");

        return parse_postfix(p, expr);
    }

    ast_error(&current, p, "Expected expression!");
    MEOW_UNREACHABLE("Should have exited in ast_error()");
}

AstNode* parse_postfix(Parser* p, AstNode* expr) {
    while (true) {
        Token current = *parser_peek(p);

        if (current.type == T_LEFTPAREN) {
            parser_advance(p);

            AstNode* call = arena_alloc(p -> arena, sizeof(*call));

            call -> kind = AST_FN_CALL;

            call -> fn_call.identifier = expr;
            call -> fn_call.args.nodes = arena_alloc(p -> arena, sizeof(AstNode*) * 8);
            call -> fn_call.args.count = 0;
            call -> fn_call.args.cap   = 8;

            if (!parser_check(p, T_RIGHTPAREN)) {
                do {
                    AstNode* node = parse_expr(p);
                    ast_vec_push(p -> arena, &call -> fn_call.args, node);

                    if (parser_check(p, T_RIGHTPAREN)) {
                        break;
                    }
                } while (parser_check_advance(p, T_COMMA));
            }

            parser_advance_expect(p, T_RIGHTPAREN, "Expected ')' after arguments");

            expr = call;
            continue;
        }

        if (current.type == T_LEFTSQUARE) {
            parser_advance(p);
            
            AstNode* index_expr = parse_expr(p);

            parser_advance_expect(p, T_RIGHTSQUARE, "Expected ']' after index");

            AstNode* index = arena_alloc(p -> arena, sizeof(*index));

            index -> kind = AST_INDEX;

            index -> index.array = expr;
            index -> index.index = index_expr;

            expr = index;
            continue;
        }

        if (current.type == T_DOT) {
            parser_advance(p);
            
            Token member = *parser_advance_expect(p, T_IDENTIFIER, "Expected identifier after '.'"); 

            AstNode* access = arena_alloc(p -> arena, sizeof(*access));

            access -> kind = AST_MEMBER_ACCESS;

            access -> member_access.object = expr;
            access -> member_access.member_ptr = member.literal;
            access -> member_access.member_len = member.len;
            access -> member_access.member_hash = hash_fnv1a(member.literal, member.len);

            expr = access;
            continue;
        }

        if (current.type == T_PLUSPLUS || current.type == T_MINUSMINUS) {
            Token op = *parser_advance(p);

            AstNode* postfix = arena_alloc(p -> arena, sizeof(*postfix));

            postfix -> kind = AST_POSTFIX;

            postfix -> postfix.operand = expr;
            postfix -> postfix.op = op.type;

            expr = postfix;

            continue;
        }

        break;
    }

    return expr;
}

AstNode* parse_return(Parser* p) {
    parser_advance(p);

    AstNode* node = arena_alloc(p -> arena, sizeof(*node));
    AstNode* expr = nullptr;

    if (!parser_check(p, T_SEMICOLON)) {
        expr = parse_expr(p);
    }

    node -> kind = AST_RETURN;

    node -> return_stmt.expression = expr;

    return node;
}



/*
*
*    Precedence
*
*/



u8 get_precedence(TokenType type) {
    MEOW_ASSERT(type < TOKEN_TYPE_COUNT, "Invalid token type");
    return PRECEDENCE_MAP[type];
}

b32 is_right_associative(TokenType type) {
    MEOW_ASSERT(type < TOKEN_TYPE_COUNT, "Invalid token type");
    return RIGHT_ASSOC_MAP[type];
}

b32 is_binary_operator(TokenType type) {
    return get_precedence(type) > 0;
}

b32 is_assignment_operator(TokenType type) {
    return get_precedence(type) == 1;
}

b32 is_prefix_operator(TokenType type) {
    switch (type) {
        case T_MINUS:
        case T_PLUS:
        case T_BANG:
        case T_BIT_NOT:
        case T_ASTERIX:
        case T_AMPERSAND:
        case T_PLUSPLUS:
        case T_MINUSMINUS:
            return true;

        default:
            return false;
    }
}

b32 is_postfix_operator(TokenType type) {
    switch (type) {
        case T_PLUSPLUS:
        case T_MINUSMINUS:
        case T_LEFTPAREN:
        case T_LEFTSQUARE:
        case T_DOT:
            return true;

        default:
            return false;
    }
}
