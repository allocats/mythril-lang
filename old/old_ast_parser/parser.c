#include "parser.h"

#include "precedence/precedence.h"
#include "precedence/types.h"

#include "../arena/arena.h"
#include "../ast/ast.h"
#include "../diagnostics/diagnostics.h"

#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

/* error handling */

void parser_recover_sync(Parser* p) {
    while (
        !parser_check(p, TOK_SEMI_COLON)    &&
        !parser_check(p, TOK_RIGHT_BRACE)   &&
        !parser_check(p, TOK_RIGHT_PAREN)   &&
        !parser_check(p, TOK_EOF) 
    ) {
        TokenKind kind = parser_peek(p) -> kind;

        if (
            (kind == TOK_IDENTIFIER) ||
            (kind == TOK_IMPORT)     ||
            (kind == TOK_STRUCT)     ||
            (kind == TOK_ENUM)       ||
            (kind == TOK_IMPL)       ||
            (kind == TOK_FUNCTION)   ||
            (kind == TOK_STATIC)     ||
            (kind == TOK_CONST)
        ) {
            break;
        }

        parser_advance(p);
    }
}

void parser_error_at_current(MythrilContext* ctx, Parser* p, const char* msg, const char* help) {
    SourceLocation location = source_location_from_token(
        p -> path,
        ctx -> buffer_start,
        parser_peek(p)
    );

    diag_error_help(ctx -> diag_ctx, location, msg, help);
}

void parser_error_after_previous(MythrilContext* ctx, Parser* p, const char* msg, const char* help) {
    SourceLocation location = source_location_from_token(
        p -> path,
        ctx -> buffer_start,
        parser_previous(p)
    );

    location.column += location.length;
    location.pointer += location.length;
    location.length = 1;

    diag_error_help(ctx -> diag_ctx, location, msg, help);
}

void parser_error_at_previous(
    MythrilContext* ctx,
    Parser* p,
    const char* message,
    const char* help
) {
    SourceLocation location = source_location_from_token(
        p -> path,
        ctx -> buffer_start,
        parser_previous(p)
    );

    diag_error_help(ctx -> diag_ctx, location, message, help);
}

AstNode* parser_fail(Parser* p, AstNode* node) {
    parser_recover_sync(p);

    node -> kind = AST_ERROR;

    return node;
}

/* parsing */ 

#define MAX_PATH_SEGMENTS 64
#define SEGMENTS_SIZE (sizeof(AstSlice) * MAX_PATH_SEGMENTS)

bool parse_path_segments(MythrilContext* ctx, Parser* p, AstSlice* segments, usize* count) {
    if (!parser_require(ctx, p, TOK_IDENTIFIER, "module name before '::'")) {
        return false;
    }

    while (!parser_check(p, TOK_SEMI_COLON)) {
        if (*count > MAX_PATH_SEGMENTS) {
            parser_error_at_current(
                ctx,
                p,
                "too many path segments",
                "module paths are limited to 64 segments, sorry"
            );
        }

        Token* segment = parser_advance(p);
        segments[(*count)++] = *ast_make_slice_from_token(p -> arena, segment);

        if (parser_check(p, TOK_SEMI_COLON)) {
            break;
        }
        
        if (!parser_expect(ctx, p, TOK_COLON_COLON, "'::' or ';'")) {
            return false;
        }

        if (!parser_require(ctx, p, TOK_IDENTIFIER, "module name after '::'")) {
            return false;
        }
    }

    parser_advance(p);

    return true;
}

AstNode* parse_module_decl(MythrilContext* ctx, Parser* p) {
    AstNode* node = arena_alloc(p -> arena, sizeof(*node));

    node -> kind = AST_MODULE_DECL;
    node -> module_decl.count = 0;
    node -> module_decl.segments = arena_alloc(p -> arena, SEGMENTS_SIZE);

    parser_advance(p);

    if (!parse_path_segments(
        ctx,
        p,
        node -> module_decl.segments,
        &node -> module_decl.count
    )) {
        return parser_fail(p, node);
    }

    return node;
}

AstNode* parse_import_decl(MythrilContext* ctx, Parser* p) {
    AstNode* node = arena_alloc(p -> arena, sizeof(*node));

    node -> kind = AST_IMPORT_DECL;
    node -> import_decl.count = 0;
    node -> import_decl.segments = arena_alloc(p -> arena, SEGMENTS_SIZE); 

    parser_advance(p);

    if (!parse_path_segments(
        ctx,
        p,
        node -> import_decl.segments,
        &node -> import_decl.count
    )) {
        return parser_fail(p, node);
    }

    return node;
}

AstNode* parse_enum_decl(MythrilContext* ctx, Parser* p) {
    AstNode* node = arena_alloc(p -> arena, sizeof(*node));

    return node;
}

AstNode* parse_struct_decl(MythrilContext* ctx, Parser* p) {
    AstNode* node = arena_alloc(p -> arena, sizeof(*node));

    return node;
}
//
// AstNode* parse_impl_fn_decl(MythrilContext* ctx, Parser* p) {
//     AstNode* node = arena_alloc(p -> arena, sizeof(*node));
//
//     node -> kind = AST_FUNCTION_DECL;
//
//     if (!parser_expect(ctx, p, TOK_FUNCTION, "'fn'")) {
//         return parser_fail(p, node);
//     }
//
//     Token* name = parser_peek(p);
//
//     if (name -> kind != TOK_IDENTIFIER) {
//         parser_error_at_current(ctx, p, "expected function name", "add name");
//         return parser_fail(p, node);
//     }
//
//     node -> function_decl.identifier = *ast_make_slice_from_token(p -> arena, name);
//
//     return node;
// }

AstNode* parse_impl_decl(MythrilContext* ctx, Parser* p) {
    AstNode* node = arena_alloc(p -> arena, sizeof(*node));
    
    node -> kind = AST_IMPL_DECL;

    node -> impl_decl.functions.count = 0;
    node -> impl_decl.functions.capacity = 8;
    node -> impl_decl.functions.items = arena_alloc(p -> arena, sizeof(AstNode*) * 8);

    parser_advance(p);

    Token* token = parser_peek(p);

    if (token -> kind != TOK_IDENTIFIER) {
        parser_error_at_current(ctx, p, "expected identifier", "add a valid identifier");
        return parser_fail(p, node);
    }

    node -> impl_decl.target = *ast_make_slice_from_token(p -> arena, token);

    parser_advance(p);

    if (!parser_expect(ctx, p, TOK_LEFT_BRACE, "'{'")) {
        return parser_fail(p, node);
    }

    while (!parser_check(p, TOK_RIGHT_BRACE)) {
        AstNode* fn = parse_function_decl(ctx, p);

        if (fn -> kind == AST_ERROR) {
            return parser_fail(p, node);
        }

        if (node -> impl_decl.functions.count >= node -> impl_decl.functions.capacity) {
            usize old_cap = node -> impl_decl.functions.capacity;
            usize new_cap = node -> impl_decl.functions.capacity * 2;

            usize old_size = old_cap * sizeof(AstNode*);
            usize new_size = new_cap * sizeof(AstNode*);

            node -> impl_decl.functions.items = arena_realloc(
                p -> arena,
                node -> impl_decl.functions.items,
                old_size,
                new_size
            );

            node -> impl_decl.functions.capacity = new_cap;
        }

        node -> impl_decl.functions.items[node -> impl_decl.functions.count++] = fn;
    }

    return node;
}

AstNode* parse_function_decl(MythrilContext* ctx, Parser* p) {
    AstNode* node = arena_alloc(p -> arena, sizeof(*node));

    node -> kind = AST_FUNCTION_DECL;
    node -> function_decl.count = 0;
    node -> function_decl.capacity = 8;
    node -> function_decl.parameters = arena_alloc(p -> arena, sizeof(AstParameter) * 8);

    parser_advance(p);

    if (!parser_require(ctx, p, TOK_IDENTIFIER, "function name")) {
        return parser_fail(p, node);
    }

    Token* name = parser_advance(p);
    node -> function_decl.identifier = *ast_make_slice_from_token(p -> arena, name);

    if (!parser_expect(ctx, p, TOK_LEFT_PAREN, "'('")) {
        return parser_fail(p, node);
    }

    while (!parser_check(p, TOK_RIGHT_PAREN)) {
        if (node -> function_decl.count >= node -> function_decl.capacity) {
            usize old_size = node -> function_decl.capacity * sizeof(AstParameter);
            usize new_size = old_size * 2;

            node -> function_decl.parameters = arena_realloc(
                p -> arena,
                node -> function_decl.parameters,
                old_size,
                new_size
            );

            node -> function_decl.capacity *= 2;
        }

        if (!parser_require(ctx, p, TOK_IDENTIFIER, "parameter name")) {
            return parser_fail(p, node);
        }

        Token* param_name = parser_advance(p);

        if (!parser_expect(ctx, p, TOK_COLON, "':' between parameter name and type")) {
            return parser_fail(p, node);
        }

        usize index = node -> function_decl.count;
        AstParameter* param = &node -> function_decl.parameters[index];

        param -> identifier = *ast_make_slice_from_token(p -> arena, param_name);
        param -> type = parse_type(ctx, p);

        if (param -> type == nullptr) {
            return parser_fail(p, node);
        }

        if (parser_check(p, TOK_RIGHT_PAREN)) {
            break;
        }

        if (!parser_expect(ctx, p, TOK_COMMA, "',' between parameters")) {
            return parser_fail(p, node);
        }
    }

    if (!parser_expect(ctx, p, TOK_RIGHT_PAREN, "')'")) {
        return parser_fail(p, node);
    }
    
    if (!parser_expect(ctx, p, TOK_COLON, "':' before return type")) {
        return parser_fail(p, node);
    }
    
    node -> function_decl.return_type = parse_type(ctx, p);

    if (node -> function_decl.return_type == NULL) {
        return parser_fail(p, node);
    }
    
    if (!parser_expect(ctx, p, TOK_LEFT_BRACE, "'{'")) {
        return parser_fail(p, node);
    }
    
    node -> function_decl.block = parse_block(ctx, p);
    
    if (!parser_expect(ctx, p, TOK_RIGHT_BRACE, "'}'")) {
        return parser_fail(p, node);
    }

    return node;
}

AstNode* parse_const_decl(MythrilContext* ctx, Parser* p) {
    AstNode* node = arena_alloc(p -> arena, sizeof(*node));

    node -> kind = AST_CONST_DECL;

    parser_advance(p);

    if (!parser_require(ctx, p, TOK_IDENTIFIER, "constant name")) {
        return parser_fail(p, node);
    }

    Token* name = parser_advance(p);

    if (!parser_expect(ctx, p, TOK_COLON, "':' between constant and type")) {
        return parser_fail(p, node);
    }

    node -> const_decl.identifier = *ast_make_slice_from_token(p -> arena, name);
    node -> const_decl.type = parse_type(ctx, p);
    node -> const_decl.value = nullptr;

    if (node -> const_decl.type == nullptr) {
        return parser_fail(p, node);
    }

    if (!parser_expect(ctx, p, TOK_EQUALS, "'=' constant must have constant value")) {
        return parser_fail(p, node);
    }

    node -> const_decl.value = parse_expression(ctx, p);

    // if (!parser_expect(ctx, p, TOK_SEMI_COLON, "';'")) {
    //     return parser_fail(p, node);
    // }

    return node;
}

AstNode* parse_static_decl(MythrilContext* ctx, Parser* p) {
    AstNode* node = arena_alloc(p -> arena, sizeof(*node));

    node -> kind = AST_STATIC_DECL;

    parser_advance(p);

    if (!parser_require(ctx, p, TOK_IDENTIFIER, "variable name")) {
        return parser_fail(p, node);
    }

    Token* name = parser_advance(p);

    if (!parser_expect(ctx, p, TOK_COLON, "':' between identifer and type")) {
        return parser_fail(p, node);
    }

    node -> static_decl.identifier = *ast_make_slice_from_token(p -> arena, name);
    node -> static_decl.type = parse_type(ctx, p);
    node -> static_decl.value = nullptr;

    if (node -> static_decl.type == nullptr) {
        return parser_fail(p, node);
    }

    if (parser_check(p, TOK_SEMI_COLON)) {
        parser_advance(p);
        return node;
    }

    if (!parser_expect(ctx, p, TOK_EQUALS, "'=' or ';'")) {
        return parser_fail(p, node);
    }

    node -> static_decl.value = parse_expression(ctx, p);

    return node;
}

AstNode* parse_var_decl(MythrilContext* ctx, Parser *p) {
    AstNode* node = arena_alloc(p -> arena, sizeof(*node));

    node -> kind = AST_VAR_DECL;

    parser_advance(p);

    if (!parser_require(ctx, p, TOK_IDENTIFIER, "variable name")) {
        return parser_fail(p, node);
    }

    Token* name = parser_advance(p);

    if (!parser_expect(ctx, p, TOK_COLON, "':' between identifer and type")) {
        return parser_fail(p, node);
    }

    node -> var_decl.identifier = *ast_make_slice_from_token(p -> arena, name);
    node -> var_decl.type = parse_type(ctx, p);
    node -> var_decl.value = nullptr;

    if (node -> var_decl.type == nullptr) {
        return parser_fail(p, node);
    }

    if (parser_check(p, TOK_SEMI_COLON)) {
        parser_advance(p);
        return node;
    }

    if (!parser_expect(ctx, p, TOK_EQUALS, "'=' or ';'")) {
        return parser_fail(p, node);
    }

    node -> var_decl.value = parse_expression(ctx, p);

    return node;
}

AstVec parse_block(MythrilContext* ctx, Parser* p) {
    AstVec vec = {
        .items = arena_alloc(p -> arena, sizeof(AstNode*) * 8),
        .count = 0,
        .capacity = 8
    };

    // NEED TO ADD DEPTH FOR BRACES/SCOPE

    while (!parser_check(p, TOK_RIGHT_BRACE)) {
        AstNode* statement = parse_statement(ctx, p);

        if (vec.count >= vec.capacity) {
            usize size = vec.count * sizeof(*vec.items);

            vec.items = arena_realloc(p -> arena, vec.items, size, size * 2);
            vec.capacity *= 2;
        }

        vec.items[vec.count++] = statement;
    }

    if (vec.count == 0) {
        SourceLocation location = source_location_from_token(
            ctx -> diag_ctx -> path,
            ctx -> buffer_start,
            parser_previous(p) 
        );

        diag_warning(ctx -> diag_ctx, location, "empty function block");
    }

    return vec;
}

AstNode* parse_statement(MythrilContext* ctx, Parser* p) {
    AstNode* node = nullptr;

    bool needs_semicolon = true;

    switch (parser_peek(p) -> kind) {
        case TOK_LET: {
            node = parse_var_decl(ctx, p);
        } break;

        case TOK_CONST: {
            node = parse_const_decl(ctx, p);
        } break;

        case TOK_IDENTIFIER: {
            node = parse_expression(ctx, p);
        } break;

        case TOK_LOOP: {
            node = parse_loop_stmt(ctx, p);
            needs_semicolon = false;
        } break;

        case TOK_WHILE: {
            node = parse_while_stmt(ctx, p);
            needs_semicolon = false;
        } break;

        case TOK_FOR: {
            node = parse_for_stmt(ctx, p);
            needs_semicolon = false;
        } break;

        case TOK_RETURN: {
            node = parse_return_stmt(ctx, p);
        } break;

        case TOK_EOF: {
            parser_advance(p);
        } break;

        case TOK_EOP: {
            return node;
        } break;

        default: {
            node = arena_alloc(p -> arena, sizeof(*node));
            parser_error_at_current(ctx, p, "unexpected statement", "plink");
            return parser_fail(p, node);
        } break;
    }

    if (needs_semicolon && !parser_expect(ctx, p, TOK_SEMI_COLON, "';'")) {
        return parser_fail(p, node);
    }

    return node;
}


AstNode* parse_loop_stmt(MythrilContext* ctx, Parser* p) {
    AstNode* node = arena_alloc(p -> arena, sizeof(*node));

    parser_advance(p);

    node -> kind = AST_LOOP_STMT;

    if (!parser_check(p, TOK_LEFT_BRACE)) {
        parser_error_at_current(
            ctx,
            p,
            "expected '{'",
            "add '{' here (loop takes no condition)"
        );

        return parser_fail(p, node);
    }

    parser_advance(p);

    node -> loop_stmt.block = parse_block(ctx, p);

    return node;
}

AstNode* parse_while_stmt(MythrilContext* ctx, Parser* p) {
    AstNode* node = arena_alloc(p -> arena, sizeof(*node));

    parser_advance(p);

    node -> kind = AST_WHILE_STMT;

    if (parser_check(p, TOK_LEFT_BRACE)) {
        parser_error_at_previous(
            ctx,
            p,
            "expected expression",
            "use 'loop' if you want an infinite loop"
        );

        return parser_fail(p, node);
    }

    node -> while_stmt.cond = parse_expression(ctx, p);

    if (node -> while_stmt.cond -> kind == AST_ERROR) {
        return parser_fail(p, node);
    }

    if (!parser_expect(ctx, p, TOK_LEFT_BRACE, "'{' after while loop condition")) {
        return parser_fail(p, node);
    }

    node -> while_stmt.block = parse_block(ctx, p);

    return node;
}

AstNode* parse_for_stmt(MythrilContext* ctx, Parser* p) {
    AstNode* node = arena_alloc(p -> arena, sizeof(*node));

    node -> kind = AST_FOR_STMT;

    // todo: for loop 
    // "for i: i32 = 0; i < 20; i++ {}"


    node -> for_stmt.init = parse_var_decl(ctx, p);

    if (!parser_expect(ctx, p, TOK_SEMI_COLON, "';'")) {
        return parser_fail(p, node);
    }

    node -> for_stmt.cond = parse_expression(ctx, p);

    if (!parser_expect(ctx, p, TOK_SEMI_COLON, "';'")) {
        return parser_fail(p, node);
    }

    node -> for_stmt.step = parse_expression(ctx, p);

    if (!parser_expect(ctx, p, TOK_LEFT_BRACE, "'{'")) {
        return parser_fail(p, node);
    }

    node -> for_stmt.block = parse_block(ctx, p);

    return node;
}

AstNode* parse_expression(MythrilContext* ctx, Parser* p) {
    AstNode* node = parse_expr_prec(ctx, p, 0);

    if (node -> kind == AST_ERROR) {
        return parser_fail(p, node);
    }

    return node;
}

AstNode* parse_expr_prec(MythrilContext* ctx, Parser* p, u32 min_prec) {
    AstNode* left = parse_primary(ctx, p);

    while (is_binary_operator(parser_peek(p) -> kind)) {
        TokenKind op_kind = parser_peek(p) -> kind;
        u8 prec = get_precedence(op_kind);

        if (prec < min_prec) {
            break;
        }

        Token* op = parser_advance(p);

        u8 next_prec = prec + (is_right_associative(op_kind) ? 0 : 1);

        AstNode* right = parse_expr_prec(ctx, p, next_prec);

        AstNode* bin_op = arena_alloc(p -> arena, sizeof(*bin_op));

        bin_op -> kind = AST_BINARY;
        bin_op -> binary.left = left;
        bin_op -> binary.op = op -> kind;
        bin_op -> binary.right = right;

        left = bin_op;
    }

    return left;
}

AstNode* parse_primary(MythrilContext* ctx, Parser* p) {
    Token current = *parser_peek(p);

    AstNode* node = arena_alloc(p -> arena, sizeof(*node));

    if (is_prefix_operator(current.kind)) {
        Token op = *parser_advance(p);

        AstNode* operand = parse_expr_prec(ctx, p, UNARY_PRECEDENCE);

        AstNode* unary_op = arena_alloc(p -> arena, sizeof(*unary_op));

        unary_op -> kind = AST_UNARY;

        unary_op -> unary.op = op.kind;
        unary_op -> unary.operand = operand;

        return parse_postfix(ctx, p, unary_op);
    }

    if (current.kind == TOK_LITERAL_NUMBER) {
        Token token = *parser_advance(p);
        
        node -> kind = AST_LITERAL;

        node -> literal.kind = TOK_LITERAL_NUMBER;
        node -> literal.value = *ast_make_slice_from_token(p -> arena, &token);

        return parse_postfix(ctx, p, node);
    }

    if (current.kind == TOK_LITERAL_STRING) {
        Token token = *parser_advance(p);
        
        node -> kind = AST_LITERAL;

        node -> literal.kind = TOK_LITERAL_STRING;
        node -> literal.value = *ast_make_slice_from_token(p -> arena, &token);

        return parse_postfix(ctx, p, node);
    }

    if (current.kind == TOK_TRUE || current.kind == TOK_FALSE) {
        Token token = *parser_advance(p);
        
        node -> kind = AST_LITERAL;

        node -> literal.kind = token.kind;
        node -> literal.value = *ast_make_slice_from_token(p -> arena, &token);

        return parse_postfix(ctx, p, node);
    }

    if (current.kind == TOK_NULL) {
        Token token = *parser_advance(p);
        
        AstNode* node = arena_alloc(p -> arena, sizeof(*node));

        node -> kind = AST_LITERAL;

        node -> literal.kind = TOK_NULL;
        node -> literal.value = *ast_make_slice_from_token(p -> arena, &token);

        return parse_postfix(ctx, p, node);
    }

    if (current.kind == TOK_IDENTIFIER) {
        Token token = *parser_advance(p);
        
        node -> kind = AST_IDENTIFIER;

        node -> identifier.value = *ast_make_slice_from_token(p -> arena, &token);

        return parse_postfix(ctx, p, node);
    }

    if (current.kind == TOK_LEFT_PAREN) {
        parser_advance(p);

        AstNode* expr = parse_expression(ctx, p);

        if (!parser_expect(ctx, p, TOK_RIGHT_PAREN, "')' after expression")) {
            return parser_fail(p, expr);
        }

        return parse_postfix(ctx, p, expr);
    }

    parser_error_after_previous(
        ctx,
        p,
        "expected an expression",
        "add an expression"
    );

    return parser_fail(p, node);
}

AstNode* parse_postfix(MythrilContext* ctx, Parser* p, AstNode* node) {
    while (true) {
        Token current = *parser_peek(p);

        if (current.kind == TOK_LEFT_PAREN) {
            parser_advance(p);

            AstNode* call = arena_alloc(p -> arena, sizeof(*call));

            call -> kind = AST_FUNCTION_CALL;

            call -> function_call.identifier = node -> identifier.value;
            call -> function_call.args.items = arena_alloc(p -> arena, sizeof(AstNode*) * 8);
            call -> function_call.args.count = 0;
            call -> function_call.args.capacity = 8;

            while (!parser_check(p, TOK_RIGHT_PAREN)) {
                AstNode* argument = parse_expression(ctx, p);

                if (call -> function_call.args.count >= call -> function_call.args.capacity) {
                    usize old_size = sizeof(AstNode*) * call -> function_call.args.capacity;
                    usize new_size = old_size * 2;

                    call -> function_call.args.items = arena_realloc(
                        p -> arena,
                        call -> function_call.args.items,
                        old_size,
                        new_size
                    );
                }

                call -> function_call.args.items[call -> function_call.args.count++] = argument;

                if (parser_check(p, TOK_RIGHT_PAREN)) {
                    break;
                }

                if (!parser_expect(ctx, p, TOK_COMMA, "',' between arguments")) {
                    return parser_fail(p, node);
                }
            }

            parser_expect(ctx, p, TOK_RIGHT_PAREN, "')' after arguments");

            node = call;
            continue;
        }

        if (current.kind == TOK_LEFT_SQUARE) {
            parser_advance(p);
            
            AstNode* index_expr = parse_expression(ctx, p);

            if (!parser_expect(ctx, p, TOK_RIGHT_SQUARE, "expected ']' after index")) {
                return parser_fail(p, node);
            }

            AstNode* index = arena_alloc(p -> arena, sizeof(*index));

            index -> kind = AST_ARRAY_INDEX;

            index -> array_index.array = node;
            index -> array_index.index = index_expr;

            node = index;
            continue;
        }

        if (current.kind == TOK_DOT) {
            parser_advance(p);
            
            Token member = *parser_advance(p); 

            if (member.kind != TOK_IDENTIFIER) {
                parser_error_at_current(ctx, p, "expected identifier", "add object identifier");
                return parser_fail(p, node);
            }

            AstNode* access = arena_alloc(p -> arena, sizeof(*access));

            access -> kind = AST_MEMBER_ACCESS;

            access -> member_access.object = node;
            access -> member_access.member = *ast_make_slice_from_token(p -> arena, &member);

            node = access;
            continue;
        }

        if (current.kind == TOK_PLUS_PLUS || current.kind == TOK_MINUS_MINUS) {
            // Token op = *parser_advance(p);
            //
            // AstNode* postfix = arena_alloc(p -> arena, sizeof(*postfix));
            //
            // postfix -> kind = AST_POSTFIX;
            //
            // postfix -> postfix.operand = expr;
            // postfix -> postfix.op = op.type;
            //
            // expr = postfix;
            //
            // continue;
        }

        break;
    }

    return node;
}

AstNode* parse_return_stmt(MythrilContext* ctx, Parser* p) {
    AstNode* node = arena_alloc(p -> arena, sizeof(*node));

    node -> kind = AST_RETURN_STMT;
    node -> return_stmt.expression = nullptr;

    parser_advance(p);

    if (!parser_check(p, TOK_SEMI_COLON)) {
        node -> return_stmt.expression = parse_expression(ctx, p);

        if (node -> return_stmt.expression -> kind == AST_ERROR) {
            return parser_fail(p, node);
        }
    } 

    return node;
}

AstType* parse_type(MythrilContext* ctx, Parser* p) {
    if (!parser_require(ctx, p, TOK_IDENTIFIER, "type identifier")) {
        parser_recover_sync(p);
        return NULL;
    }
    
    Token* base_token = parser_advance(p);

    AstType* base_type = arena_alloc(p -> arena, sizeof(*base_type));
    base_type -> kind = TYPE_BASIC;
    base_type -> identifier = *ast_make_slice_from_token(p -> arena, base_token);
    
    AstType* result = base_type;
    
    while (true) {
        if (parser_check(p, TOK_STAR)) {
            parser_advance(p);
            
            AstType* ptr_type = arena_alloc(p -> arena, sizeof(*ptr_type));

            ptr_type -> kind = TYPE_POINTER;
            ptr_type -> pointee = result;

            result = ptr_type;
            
        } else if (parser_check(p, TOK_LEFT_SQUARE)) {
            parser_advance(p);
            
            AstType* array_type = arena_alloc(p -> arena, sizeof(*array_type));

            array_type -> kind = TYPE_ARRAY;
            array_type -> array.element_type = result;
            
            if (!parser_check(p, TOK_RIGHT_SQUARE)) {
                array_type -> array.size_expr = parse_expression(ctx, p);
                
                if (array_type -> array.size_expr == NULL) {
                    parser_error_at_current(
                        ctx,
                        p,
                        "expected array size expression or ']'",
                        "provide constant expression like 10 or 'BUFFER_SIZE'" 
                    );
                    parser_recover_sync(p);
                    return NULL;
                }
            } else {
                array_type -> array.size_expr = NULL;
            }
            
            if (!parser_expect(ctx, p, TOK_RIGHT_SQUARE, "']'")) {
                parser_recover_sync(p);

                return NULL;
            }

            parser_advance(p);
            
            result = array_type;
            
        } else {
            break;
        }
    }
    
    return result;
}


u64 parse_integer(char* ptr, usize len) {
    char buffer[32];

    memcpy(buffer, ptr, len);
    buffer[len] = 0;

    return strtoull(buffer, NULL, 10);
}

bool parser_expect(MythrilContext* ctx, Parser* p, TokenKind kind, const char* what) {
    if (!parser_check(p, kind)) {
        char msg[128];
        char help[128];

        snprintf(msg, sizeof(msg), "expected %s", what);
        snprintf(help, sizeof(help), "add %s here", what);

        parser_error_after_previous(ctx, p, msg, help);

        return false;
    }

    parser_advance(p);

    return true;
}

bool parser_require(MythrilContext* ctx, Parser* p, TokenKind kind, const char* what) {
    if (!parser_check(p, kind)) {
        char msg[128];
        char help[128];

        snprintf(msg, sizeof(msg), "expected %s", what);
        snprintf(help, sizeof(help), "add %s", what);

        parser_error_at_current(ctx, p, msg, help);

        return false;
    }

    return true;
}

inline Token* parser_previous(Parser* p) {
    if (p -> index <= 0) {
        fprintf(stderr, "error: tokens underflow OOB\n");
        exit(1);
    }

    return &p -> tokens -> items[p -> index - 1];
}

inline Token* parser_peek(Parser* p) {
    if (p -> index >= p -> count) {
        fprintf(stderr, "error: sudden end of tokens OOB\n");
        exit(1);
    }

    return &p -> tokens -> items[p -> index];
}

inline Token* parser_peek_ahead(Parser* p) {
    if (p -> index >= p -> count) {
        fprintf(stderr, "error: sudden end of tokens OOB\n");
        exit(1);
    }

    return &p -> tokens -> items[p -> index + 1];
}

inline Token* parser_advance(Parser* p) {
    if (p -> index >= p -> count) {
        fprintf(stderr, "error: sudden end of tokens OOB\n");
        exit(1);
    }

    return &p -> tokens -> items[p -> index++];
}

inline bool parser_check(Parser* p, TokenKind kind) {
    Token* tok = parser_peek(p);
    return tok -> kind == kind;
}

inline bool parser_check_ahead(Parser* p, TokenKind kind) {
    Token* tok = parser_peek_ahead(p);
    return tok -> kind == kind;
}
