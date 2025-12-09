#include "defaults.h"
#include "parser.h"
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

bool parse_path_segments(MythrilContext* ctx, Parser* p, AstSlice* segments, usize* count) {
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

        parser_advance(p);

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

    if (!parser_check_current(p, TOK_LEFT_BRACE)) {
        error_at_previous_end(
            ctx,
            p,
            "expected '{'",
            "add '{' after " 
        );
    }

    parser_advance(p);

    // todo: parse block

    return node;
}

AstNode* parse_const_decl(MythrilContext* ctx, Parser* p) {
    AstNode* node = arena_alloc(p -> arena, sizeof(*node));

    return node;
}

AstNode* parse_static_decl(MythrilContext* ctx, Parser* p) {
    AstNode* node = arena_alloc(p -> arena, sizeof(*node));

    return node;
}

AstNode* parse_var_decl(MythrilContext* ctx, Parser *p) {
    AstNode* node = arena_alloc(p -> arena, sizeof(*node));

    return node;
}


AstNode* parse_statement(MythrilContext* ctx, Parser* p) {
    AstNode* node = arena_alloc(p -> arena, sizeof(*node));

    return node;
}

AstNode* parse_loop_stmt(MythrilContext* ctx, Parser* p) {
    AstNode* node = arena_alloc(p -> arena, sizeof(*node));

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
