#include "defaults.h"
#include "parser.h"
#include "errors/errors.h"
#include "recovery/recovery.h"
#include "types.h"

#include "precedence/precedence.h"

#include "../ast/ast.h"

#include <stdio.h>
#include <stdbool.h>

AstNode* top_level_decl_fail(Parser* p, AstNode* node) {
    recover_to_top_level_decl(p);
    node -> kind = AST_ERROR;
    return node;
}

AstNode* statement_fail(Parser* p, AstNode* node) {
    recover_in_fn_body(p);
    node -> kind = AST_ERROR;
    return node;
}

b8 parse_path_segments(MythrilContext* ctx, Parser* p, AstSlice* segments, usize* count) {
    while (!parser_check_current(p, TOK_SEMI_COLON)) {
        if (*count > MAX_PATH_SEGMENTS) {
            error_till_end_of_line(
                ctx,
                p,
                "too many path segments, max 32",
                "reduce pathing"
            );

            return false;
        }

        Token* segment = parser_peek(p);

        if (segment -> kind != TOK_IDENTIFIER) {
            error_at_previous_end(
                ctx,
                p,
                "expected path name",
                "add a path name here"
            );

            return false;
        }

        segments[(*count)++] = *make_slice_from_token(p -> arena, segment);

        parser_advance(p);

        Token* token = parser_peek(p);

        if (token -> kind == TOK_SEMI_COLON) {
            parser_advance(p);
            return true;
        }

        if (token -> kind != TOK_COLON_COLON) {
            error_at_previous_end(
                ctx,
                p,
                "expected '::' or ';' after segment",
                "add '::' or ';' here"
            );

            return false;
        }

        parser_advance(p);
    }

    parser_advance(p);

    return true;
}

AstNode* parse_module_decl(MythrilContext* ctx, Parser* p) {
    AstNode* node = arena_alloc(p -> arena, sizeof(*node));

    node -> kind = AST_MODULE_DECL;

    node -> module_decl.segments = arena_alloc(p -> arena, SEGMENTS_SIZE);
    node -> module_decl.count = 0;

    b8 result = parse_path_segments(
        ctx,
        p,
        node -> module_decl.segments,
        &node -> module_decl.count
    );

    if (!result) {
        return top_level_decl_fail(p, node);
    }

    return node;
}

AstNode* parse_import_decl(MythrilContext* ctx, Parser* p) {
    AstNode* node = arena_alloc(p -> arena, sizeof(*node));

    node -> kind = AST_IMPORT_DECL;

    node -> import_decl.segments = arena_alloc(p -> arena, SEGMENTS_SIZE);
    node -> import_decl.count = 0;

    b8 result = parse_path_segments(
        ctx,
        p,
        node -> import_decl.segments,
        &node -> import_decl.count
    );

    if (!result) {
        return top_level_decl_fail(p, node);
    }

    return node;
}

AstNode* parse_enum_decl(MythrilContext* ctx, Parser* p) {
    AstNode* node = arena_alloc(p -> arena, sizeof(*node));

    node -> kind = AST_ENUM_DECL;
    node -> enum_decl.capacity = ENUM_INIT_CAP;
    node -> enum_decl.count = 0;
    node -> enum_decl.variants = arena_alloc(
        p -> arena,
        sizeof(AstEnumVariant) * ENUM_INIT_CAP
    );

    Token* name = parser_peek(p); 

    if (name -> kind != TOK_IDENTIFIER) {
        error_at_current(
            ctx,
            p,
            "expected enum name",
            "add a valid name here"
        );

        return top_level_decl_fail(p, node);
    }

    parser_advance(p);

    if (!parser_check_current(p, TOK_LEFT_BRACE)) {
        error_at_previous_end(
            ctx,
            p,
            "expected '{'",
            "add '{' here"
        );

        return top_level_decl_fail(p, node);
    }

    delimiters_push(p, parser_peek(p), name -> lexeme, name -> length);

    parser_advance(p);

    if (parser_check_current(p, TOK_RIGHT_BRACE)) {
        delimiters_pop(p);
        parser_advance(p);
        return node;
    }

    while (!parser_check_current(p, TOK_RIGHT_BRACE)) {

    }

    return node;
}

AstNode* parse_struct_decl(MythrilContext* ctx, Parser* p) {
    AstNode* node = arena_alloc(p -> arena, sizeof(*node));

    return node;
}

AstNode* parse_impl_decl(MythrilContext* ctx, Parser* p) {
    AstNode* node = arena_alloc(p -> arena, sizeof(*node));

    return node;
}

/*

    TODO: Error recovery and handling, block parsing

*/

AstNode* parse_function_decl(MythrilContext* ctx, Parser* p) {
    AstNode* node = arena_alloc(p -> arena, sizeof(*node));

    node -> kind = AST_FUNCTION_DECL;

    node -> function_decl.param_count = 0;
    node -> function_decl.param_capacity = PARAM_INIT_CAPACITY;
    node -> function_decl.parameters = arena_alloc(
        p -> arena,
        sizeof(AstParameter) * PARAM_INIT_CAPACITY
    );

    node -> function_decl.stmt_count = 0;
    node -> function_decl.stmt_capacity = STMTS_INIT_CAPACITY;
    node -> function_decl.statements = arena_alloc(
        p -> arena,
        sizeof(AstNode*) * STMTS_INIT_CAPACITY
    );

    Token* name = parser_peek(p);

    if (name -> kind != TOK_IDENTIFIER) {
        error_at_current(
            ctx,
            p,
            "expected function name",
            "add function name here"
        );

        return top_level_decl_fail(p, node);
    }

    node -> function_decl.identifier = *make_slice_from_token(p -> arena, name);

    parser_advance(p);

    if (!parser_check_current(p, TOK_LEFT_PAREN)) {
        error_at_previous_end(
            ctx,
            p,
            "expected '('",
            "add a '(' here" 
        );

        recover_in_fn_params(p);

        if (parser_check_current(p, TOK_COMMA)) {
            parser_advance(p);
        }
    } else {
        parser_advance(p);
    }

    // parameters
    while (!parser_check_current(p, TOK_RIGHT_PAREN)) {
        Token* param_name = parser_peek(p);

        if (param_name -> kind != TOK_IDENTIFIER) {
            error_at_previous_end(
                ctx,
                p,
                "expected parameter name",
                "add a parameter name here" 
            );

            recover_in_fn_params(p);

            if (parser_check_current(p, TOK_COMMA)) {
                parser_advance(p);
            }

            continue;
        }

        parser_advance(p);

        if (!parser_check_current(p, TOK_COLON)) {
            error_at_previous_end(
                ctx,
                p,
                "expected ':'",
                "add a ':' here between the parameter name and type" 
            );

            recover_in_fn_params(p);

            if (parser_check_current(p, TOK_COMMA)) {
                parser_advance(p);
            }

            continue;
        }

        parser_advance(p);

        AstType* param_type = parse_type(ctx, p);

        if (param_type == nullptr) {
            recover_in_fn_params(p);
            continue;
        }

        node -> function_decl.parameters[node -> function_decl.param_count++] = (AstParameter) {
            .identifier = *make_slice_from_token(p -> arena, param_name),
            .type = param_type
        };

        // parser_advance(p);

        TokenKind kind = parser_peek(p) -> kind;

        if (kind == TOK_RIGHT_PAREN) {
            break;
        }

        if (kind != TOK_COMMA) {
            error_at_previous_end(
                ctx,
                p,
                "expected ','",
                "add ',' between parameters"
            );

            recover_in_fn_params(p);
            continue;
        }

        parser_advance(p);
    }

    parser_advance(p);

    if (!parser_check_current(p, TOK_COLON)) {
        error_at_previous_end(
            ctx,
            p,
            "expected ':'",
            "add ':' after paramaters before return type"
        );

        node -> function_decl.return_type = nullptr;
    }

    parser_advance(p);

    AstType* return_type = parse_type(ctx, p);
    
    if (return_type == nullptr) {
        return top_level_decl_fail(p, node);
    }

    node -> function_decl.return_type = return_type;

    if (!parser_check_current(p, TOK_LEFT_BRACE)) {
        error_at_previous_end(
            ctx,
            p,
            "expected '{'",
            "add '{' after " 
        );
    }

    parser_advance(p);

    // todo: parse block, with depth ADD DEPTH ASAP

    AstFunctionDecl* function = &node -> function_decl;

    while (!parser_check_current(p, TOK_RIGHT_BRACE)) {
        if (function -> stmt_count >= function -> stmt_capacity) {
            usize size = function -> stmt_capacity * sizeof(AstNode*);

            function -> statements = arena_realloc(
                p -> arena,
                function -> statements,
                size,
                size * 2
            );

            function -> stmt_capacity *= 2;
        }

        AstNode* statement = parse_statement(ctx, p);

        function -> statements[function -> stmt_count++] = statement;
    }

    parser_advance(p);

    return node;
}

AstNode* parse_const_decl(MythrilContext* ctx, Parser* p) {
    AstNode* node = arena_alloc(p -> arena, sizeof(*node));

    node -> kind = AST_CONST_DECL;

    Token* name = parser_advance(p);

    if (name -> kind != TOK_IDENTIFIER) {
        error_at_previous(
            ctx,
            p,
            "expected identifier",
            "add const name"
        );

        // todo: add flag or parser state machine, to indicate if in global space or not etc.
        // then add an if statement here or a general decl_fail() function that can deduce
        // the correct recovery method

        return top_level_decl_fail(p, node);
    }

    if (!parser_check_current(p, TOK_COLON)) {
        error_at_previous_end(
            ctx,
            p,
            "expected ':', const must have type",
            "add ':' here"
        );

        return top_level_decl_fail(p, node);
    }

    parser_advance(p);

    AstType* type = parse_type(ctx, p);

    if (!type) {
        return top_level_decl_fail(p, node);
    }

    if (!parser_check_current(p, TOK_EQUALS)) {
        error_at_previous_end(
            ctx,
            p,
            "expected '='",
            "const must be initialised, add '=' and expression here"
        );

        return top_level_decl_fail(p, node);
    }

    parser_advance(p);

    AstNode* value = parse_expression(ctx, p);

    if (value == nullptr || value -> kind == AST_ERROR) {
        return top_level_decl_fail(p, node);
    }

    if (!parser_check_current(p, TOK_SEMI_COLON)) {
        error_at_previous_end(
            ctx,
            p,
            "expected ';'",
            "add ';' after constant declaration"
        );

        return top_level_decl_fail(p, node);
    }

    parser_advance(p);

    node -> const_decl.identifier = *make_slice_from_token(p -> arena, name);
    node -> const_decl.type = type;
    node -> const_decl.value = value;

    return node;
}

AstNode* parse_static_decl(MythrilContext* ctx, Parser* p) {
    AstNode* node = arena_alloc(p -> arena, sizeof(*node));

    node -> kind = AST_STATIC_DECL;

    Token* name = parser_advance(p);

    if (name -> kind != TOK_IDENTIFIER) {
        error_at_previous(
            ctx,
            p,
            "expected identifier",
            "add a name here"
        );

        // todo: add flag or parser state machine, to indicate if in global space or not etc.
        // then add an if statement here or a general decl_fail() function that can deduce
        // the correct recovery method

        return top_level_decl_fail(p, node);
    }

    if (!parser_check_current(p, TOK_COLON)) {
        error_at_previous_end(
            ctx,
            p,
            "expected ':', must have type",
            "add ':' here"
        );

        return top_level_decl_fail(p, node);
    }

    parser_advance(p);

    AstType* type = parse_type(ctx, p);

    if (!type) {
        return top_level_decl_fail(p, node);
    }

    if (!parser_check_current(p, TOK_EQUALS)) {
        error_at_previous_end(
            ctx,
            p,
            "expected '='",
            "const must be initialised, add '=' and expression here"
        );

        return top_level_decl_fail(p, node);
    }

    parser_advance(p);

    AstNode* value = parse_expression(ctx, p);

    if (value == nullptr || value -> kind == AST_ERROR) {
        return top_level_decl_fail(p, node);
    }

    if (!parser_check_current(p, TOK_SEMI_COLON)) {
        error_at_previous_end(
            ctx,
            p,
            "expected ';'",
            "add ';' after constant declaration"
        );

        return top_level_decl_fail(p, node);
    }

    parser_advance(p);

    node -> const_decl.identifier = *make_slice_from_token(p -> arena, name);
    node -> const_decl.type = type;
    node -> const_decl.value = value;

    return node;
}

AstNode* parse_var_decl(MythrilContext* ctx, Parser *p) {
    AstNode* node = arena_alloc(p -> arena, sizeof(*node));

    node -> kind = AST_VAR_DECL;

    node -> var_decl.value = nullptr;

    Token* name = parser_peek(p);

    if (name -> kind != TOK_IDENTIFIER) {
        error_at_current(
            ctx,
            p,
            "expected variable name",
            "add a valid name here" 
        );

        return statement_fail(p, node);
    }

    node -> var_decl.identifier = *make_slice_from_token(p -> arena, name);

    parser_advance(p);

    if (!parser_check_current(p, TOK_COLON)) {
        error_at_previous_end(
            ctx,
            p,
            "expected ':'",
            "add ':' here between name and type"
        );

        return statement_fail(p, node);
    }

    parser_advance(p);

    AstType* type = parse_type(ctx, p);

    if (!type || type -> kind == TYPE_ERR) {
        error_whole_line(
            ctx,
            p,
            "invalid type",
            "fix variable type"
        );

        return statement_fail(p, node);
    }

    node -> var_decl.type = type;

    Token* next = parser_advance(p);

    switch (next -> kind) {
        case TOK_SEMI_COLON: {
            return node;
        } break;

        case TOK_EQUALS: {
            node -> var_decl.value = parse_expression(ctx, p);
        } break;

        default: {
            error_at_current(
                ctx,
                p,
                "unexpected token",
                "expected ';' or '='"
            );

            return statement_fail(p, node);
        } break;
    }

    return node;
}

AstNode* parse_statement(MythrilContext* ctx, Parser* p) {
    AstNode* node = nullptr; // = arena_alloc(p -> arena, sizeof(*node));

    TokenKind kind = parser_peek(p) -> kind;

    b8 needs_semicolon = true;

    switch (kind) {
        case TOK_LET: { 
            parser_advance(p);
            node = parse_var_decl(ctx, p);
        } break;

        case TOK_CONST: {
            parser_advance(p);
            node = parse_const_decl(ctx, p);
        } break;

        case TOK_STATIC: {
            parser_advance(p);
            node = parse_static_decl(ctx, p);
        } break;

        case TOK_IDENTIFIER: {
            node = parse_expression(ctx, p);
        } break;

        case TOK_LOOP: {
            parser_advance(p);

            node = parse_loop_stmt(ctx, p);
            needs_semicolon = false;
        } break;

        default: {
            error_till_end_of_line(
                ctx,
                p,
                "expected statement",
                "invalid start to statement"
            );

            return statement_fail(p, node);
        } break;
    }

    if (needs_semicolon && !parser_check_current(p, TOK_SEMI_COLON)) {
        error_at_previous_end(
            ctx,
            p,
            "expected ';'",
            "add ';' here"
        );

        return statement_fail(p, node);
    }

    parser_advance(p);

    return node;
}

AstNode* parse_loop_stmt(MythrilContext* ctx, Parser* p) {
    AstNode* node = arena_alloc(p -> arena, sizeof(*node));

    node -> kind = AST_LOOP_STMT;

    node -> loop_stmt.stmt_count = 0;
    node -> loop_stmt.stmt_capacity = STMTS_INIT_CAPACITY;
    node -> loop_stmt.statements = arena_alloc(p -> arena, sizeof(AstNode*) * STMTS_INIT_CAPACITY);

    if (!parser_check_current(p, TOK_LEFT_BRACE)) {
        error_at_previous_end(
            ctx,
            p,
            "expected '{'",
            "add a '{' here"
        );
    } else {
        parser_advance(p);
    }

    if (parser_check_current(p, TOK_RIGHT_BRACE)) {
        return node;
    }

    // todo: proper depth tracking but with global parser delimiters i think
    //       need to figure something out

    i32 depth = 1;

    while (depth > 0) {
        TokenKind current = parser_peek(p) -> kind;

        if (current == TOK_EOF || current == TOK_EOP) {
            return statement_fail(p, node);
        }

        if (current == TOK_LEFT_BRACE) {
            depth++;
        } else if (current == TOK_RIGHT_BRACE) {
            depth--;

            if (depth == 0) {
                return node;
            }
        }

        if (node -> loop_stmt.stmt_count >= node -> loop_stmt.stmt_capacity) {
            usize size = sizeof(AstNode*) * node -> loop_stmt.stmt_capacity;

            node -> loop_stmt.statements = arena_realloc(
                p -> arena,
                node -> loop_stmt.statements,
                size,
                size * 2
            );

            node -> loop_stmt.stmt_capacity *= 2;
        }

        node -> loop_stmt.statements[node -> loop_stmt.stmt_count++] = parse_statement(ctx, p);
    }

    return node;
}

AstNode* parse_while_stmt(MythrilContext* ctx, Parser* p) {
    AstNode* node = arena_alloc(p -> arena, sizeof(*node));

    return node;
}

AstNode* parse_for_stmt(MythrilContext* ctx, Parser* p) {
    AstNode* node = arena_alloc(p -> arena, sizeof(*node));

    return node;
}

AstNode* parse_return_stmt(MythrilContext* ctx, Parser* p) {
    AstNode* node = arena_alloc(p -> arena, sizeof(*node));

    node -> kind = AST_RETURN_STMT;

    AstNode* expresion = parse_expression(ctx, p);

    if (expresion == nullptr || expresion -> kind == AST_ERROR) {
        node -> kind = AST_ERROR;
    }

    node -> return_stmt.expression = expresion;

    return node;
}

AstType* parse_type(MythrilContext* ctx, Parser* p) {
    if (!parser_check_current(p, TOK_IDENTIFIER)) {
        error_at_current(
            ctx,
            p,
            "expected type identifier",
            "add a valid type identifier here"
        );

        return nullptr;
    }

    Token* base_token = parser_advance(p);

    AstType* base_type = arena_alloc(p -> arena, sizeof(*base_type));

    base_type -> kind = TYPE_BASIC;
    base_type -> identifier = *make_slice_from_token(p -> arena, base_token);

    AstType* result = base_type;

    while (true) {
        if (parser_check_current(p, TOK_STAR)) {
            parser_advance(p);
            
            AstType* ptr_type = arena_alloc(p -> arena, sizeof(*ptr_type));

            ptr_type -> kind = TYPE_POINTER;
            ptr_type -> pointee = result;

            result = ptr_type;
            
        } else if (parser_check_current(p, TOK_LEFT_SQUARE)) {
            parser_advance(p);
            
            AstType* array_type = arena_alloc(p -> arena, sizeof(*array_type));

            array_type -> kind = TYPE_ARRAY;
            array_type -> array.element_type = result;
            
            if (!parser_check_current(p, TOK_RIGHT_SQUARE)) {
                array_type -> array.size_expr = parse_expression(ctx, p);
                
                if (array_type -> array.size_expr == NULL) {
                    error_at_current(
                        ctx,
                        p,
                        "expected array size expression or ']'",
                        "provide constant expression like 10 or 'BUFFER_SIZE'" 
                    );

                    return nullptr;
                }
            } else {
                array_type -> array.size_expr = NULL;
            }
            
            if (!parser_check_current(p, TOK_RIGHT_SQUARE)) {
                return nullptr;
            }

            parser_advance(p);
            
            result = array_type;
            
        } else {
            break;
        }
    }

    return result;
}

AstNode* parse_expression(MythrilContext* ctx, Parser* p) {
    AstNode* node = parse_expr_prec(ctx, p, 0);
    
    if (!node || node == nullptr || node -> kind == AST_ERROR) {
        node -> kind = AST_ERROR;

        // todo: recover, perhaps pass in a pointer to a specific recovery function
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
        Token* op = parser_advance(p);

        AstNode* operand = parse_expr_prec(ctx, p, UNARY_PRECEDENCE);

        AstNode* unary_op = arena_alloc(p -> arena, sizeof(*unary_op));

        unary_op -> kind = AST_UNARY;

        unary_op -> unary.op = op -> kind;
        unary_op -> unary.operand = operand;

        return parse_postfix(ctx, p, unary_op);
    }

    if (current.kind == TOK_LITERAL_NUMBER) {
        Token* token = parser_advance(p);
        
        node -> kind = AST_LITERAL;

        node -> literal.kind = TOK_LITERAL_NUMBER;
        node -> literal.value = *make_slice_from_token(p -> arena, token);

        return parse_postfix(ctx, p, node);
    }

    if (current.kind == TOK_LITERAL_STRING) {
        Token* token = parser_advance(p);
        
        node -> kind = AST_LITERAL;

        node -> literal.kind = TOK_LITERAL_STRING;
        node -> literal.value = *make_slice_from_token(p -> arena, token);

        return parse_postfix(ctx, p, node);
    }

    if (current.kind == TOK_TRUE || current.kind == TOK_FALSE) {
        Token* token = parser_advance(p);
        
        node -> kind = AST_LITERAL;

        node -> literal.kind = token -> kind;
        node -> literal.value = *make_slice_from_token(p -> arena, token);

        return parse_postfix(ctx, p, node);
    }

    if (current.kind == TOK_NULL) {
        Token* token = parser_advance(p);
        
        AstNode* node = arena_alloc(p -> arena, sizeof(*node));

        node -> kind = AST_LITERAL;

        node -> literal.kind = TOK_NULL;
        node -> literal.value = *make_slice_from_token(p -> arena, token);

        return parse_postfix(ctx, p, node);
    }

    if (current.kind == TOK_IDENTIFIER) {
        Token* token = parser_advance(p);
        
        node -> kind = AST_IDENTIFIER;

        node -> identifier.value = *make_slice_from_token(p -> arena, token);

        return parse_postfix(ctx, p, node);
    }

    if (current.kind == TOK_LEFT_PAREN) {
        parser_advance(p);

        AstNode* expr = parse_expression(ctx, p);

        if (!parser_check_current(p, TOK_RIGHT_PAREN)) {
            error_at_current(
                ctx,
                p,
                "expected ')'",
                "add a ')' here"
            );

            expr -> kind = AST_ERROR;

            return expr;
        }

        return parse_postfix(ctx, p, expr);
    }

    error_at_previous_end(
        ctx,
        p,
        "expected an expression",
        "add an expression"
    );

    node -> kind = AST_ERROR;

    return node;
}

AstNode* parse_postfix(MythrilContext* ctx, Parser* p, AstNode* node) {
    while (true) {
        Token current = *parser_peek(p);

        if (current.kind == TOK_LEFT_PAREN) {
            parser_advance(p);

            AstNode* call = arena_alloc(p -> arena, sizeof(*call));

            call -> kind = AST_FUNCTION_CALL;

            call -> function_call.identifier = node -> identifier.value;
            call -> function_call.arg_count = 0;
            call -> function_call.arg_capacity = ARGS_INIT_CAPACITY;
            call -> function_call.arguments = arena_alloc(
                p -> arena,
                sizeof(AstNode*) * ARGS_INIT_CAPACITY
            );

            while (!parser_check_current(p, TOK_RIGHT_PAREN)) {
                AstNode* argument = parse_expression(ctx, p);

                if (call -> function_call.arg_count >= call -> function_call.arg_capacity) {
                    usize old_size = sizeof(AstNode*) * call -> function_call.arg_capacity;
                    usize new_size = old_size * 2;

                    call -> function_call.arguments = arena_realloc(
                        p -> arena,
                        call -> function_call.arguments,
                        old_size,
                        new_size
                    );
                }

                call -> function_call.arguments[call -> function_call.arg_count++] = argument;

                if (parser_check_current(p, TOK_RIGHT_PAREN)) {
                    break;
                }

                if (!parser_check_current(p, TOK_COMMA)) {
                    if (parser_check_current(p, TOK_SEMI_COLON)) {
                        error_at_previous_end(
                            ctx,
                            p,
                            "expected ')'",
                            "add a ')' here"
                        );
                    } else {
                        error_at_previous_end(
                            ctx,
                            p,
                            "expected ',' between args",
                            "add a ',' here"
                        );
                    }

                    node -> kind = AST_ERROR;

                    return node;
                }

                parser_advance(p);
            }

            if (!parser_check_current(p, TOK_RIGHT_PAREN)) {
                error_at_previous_end(
                    ctx,
                    p,
                    "expected ')'",
                    "add ')' after arguments"
                );
            }

            parser_advance(p);

            node = call;
            continue;
        }

        if (current.kind == TOK_LEFT_SQUARE) {
            parser_advance(p);
            
            AstNode* index_expr = parse_expression(ctx, p);

            if (!parser_check_current(p, TOK_RIGHT_SQUARE)) {
                error_at_previous_end(
                    ctx,
                    p,
                    "expected ']'",
                    "add a ']' here"
                );

                node -> kind = AST_ERROR;

                return node;
            }

            parser_advance(p);

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
                error_at_current(
                    ctx,
                    p,
                    "expected identifier",
                    "add object identifier"
                );

                node -> kind = AST_ERROR;

                return node;
            }

            AstNode* access = arena_alloc(p -> arena, sizeof(*access));

            access -> kind = AST_MEMBER_ACCESS;

            access -> member_access.object = node;
            access -> member_access.member = *make_slice_from_token(p -> arena, &member);

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

Token* parser_peek(Parser* p) {
    if (p -> index >= p -> count) {
        return &p -> tokens -> items[p -> count];
    }

    return &p -> tokens -> items[p -> index];
}

Token* parser_peek_previous(Parser* p) {
    if (p -> index >= p -> count) {
        return &p -> tokens -> items[p -> count];
    }

    return &p -> tokens -> items[p -> index - 1];
}

Token* parser_advance(Parser* p) {
    if (p -> index >= p -> count) {
        return &p -> tokens -> items[p -> count];
    }

    return &p -> tokens -> items[p -> index++];
}

b8 parser_check_current(Parser* p, TokenKind kind) {
    Token* tok = parser_peek(p);
    return tok -> kind == kind;
}
