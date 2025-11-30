#include "parser.h"

#include "../arena/arena.h"
#include "../ast/ast.h"
#include "../hash/hash.h"
#include "../diagnostics/diagnostics.h"
#include "../tokens/tokens.h"
#include "../utils/vec.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

static void parser_recover_sync(Parser* p) {
    while (
        !parser_check(p, TOK_SEMI_COLON)    &&
        !parser_check(p, TOK_RIGHT_BRACE)   &&
        !parser_check(p, TOK_EOF) 
    ) {
        if (
            parser_check(p, TOK_IMPORT)     ||
            parser_check(p, TOK_STRUCT)     ||
            parser_check(p, TOK_ENUM)       ||
            parser_check(p, TOK_IMPL)       ||
            parser_check(p, TOK_FUNCTION)   ||
            parser_check(p, TOK_STATIC)     ||
            parser_check(p, TOK_CONST)
        ) {
            break;
        }

        parser_advance(p);
    }

    if (
        parser_check(p, TOK_SEMI_COLON) ||
        parser_check(p, TOK_RIGHT_BRACE)
    ) {
        parser_advance(p);
    }
}

AstNode* parse_module_decl(MythrilContext* ctx, Parser* p) {
    AstNode* module_decl = arena_alloc(p -> arena, sizeof(*module_decl));

    module_decl -> kind = AST_MODULE_DECL;
    module_decl -> module_decl.count = 0;

    parser_advance(p);

    if (!parser_check(p, TOK_IDENTIFIER)) {
        SourceLocation location = source_location_from_token(
            p -> path,
            ctx -> buffer_start, 
            parser_previous(p) 
        );

        location.pointer += location.length + 1;
        location.column += location.length + 1;
        location.length = 1;

        diag_error_help(
            ctx -> diag_ctx,
            location,
            "expected a module name",
            "add a module name"
        );

        parser_recover_sync(p);

        module_decl -> kind = AST_ERROR;
        return module_decl;
    }

    while (!parser_check(p, TOK_SEMI_COLON)) {
        Token* segment = parser_advance(p);

        AstSlice* slice = &module_decl -> module_decl.segments[module_decl -> module_decl.count++];
        slice = ast_make_slice_from_token(p -> arena, segment);

        if (parser_check(p, TOK_SEMI_COLON)) {
            break;
        }

        if (!parser_check(p, TOK_COLON_COLON)) {
            SourceLocation location = source_location_from_token(
                p -> path,
                ctx -> buffer_start, 
                parser_previous(p) 
            );

            location.column += location.length;
            location.pointer += location.length;
            location.length = 1;

            diag_error_help(
                ctx -> diag_ctx,
                location,
                "expected '::' or ';' after module name",
                "add '::' to continue module path or ';' to end the declaration"
            );

            parser_recover_sync(p);

            module_decl -> kind = AST_ERROR;
            return module_decl;
        }

        parser_advance(p);

        if (!parser_check(p, TOK_IDENTIFIER)) {
            SourceLocation location = source_location_from_token(
                p -> path,
                ctx -> buffer_start, 
                parser_peek(p) 
            );

            diag_error_help(
                ctx -> diag_ctx,
                location,
                "expected module name after '::'",
                "add a module name"
            );

            parser_recover_sync(p);
            module_decl -> kind = AST_ERROR;
            return module_decl;
        }
    }

    parser_advance(p);

    return module_decl;
}

AstNode* parse_import_decl(MythrilContext* ctx, Parser* p) {
    AstNode* import_decl = arena_alloc(p -> arena, sizeof(*import_decl));

    import_decl -> kind = AST_IMPORT_DECL;
    import_decl -> import_decl.count = 0;

    parser_advance(p);

    if (!parser_check(p, TOK_IDENTIFIER)) {
        SourceLocation location = source_location_from_token(
            p -> path,
            ctx -> buffer_start, 
            parser_previous(p) 
        );

        location.pointer += location.length + 1;
        location.column += location.length + 1;
        location.length = 1;

        diag_error_help(
            ctx -> diag_ctx,
            location,
            "expected a module name",
            "try adding a module name"
        );

        parser_recover_sync(p);

        import_decl -> kind = AST_ERROR;
        return import_decl;
    }

    while (!parser_check(p, TOK_SEMI_COLON)) {
        Token* segment = parser_advance(p);

        AstSlice* slice = &import_decl -> import_decl.segments[import_decl -> import_decl.count++];
        slice = ast_make_slice_from_token(p -> arena, segment);

        if (parser_check(p, TOK_SEMI_COLON)) {
            break;
        }

        if (!parser_check(p, TOK_COLON_COLON)) {
            SourceLocation location = source_location_from_token(
                p -> path,
                ctx -> buffer_start, 
                parser_previous(p) 
            );

            location.column += location.length;
            location.pointer += location.length;
            location.length = 1;

            diag_error_help(
                ctx -> diag_ctx,
                location,
                "expected '::' or ';' after module name",
                "add '::' to continue module path or ';' to end the import"
            );

            parser_recover_sync(p);

            import_decl -> kind = AST_ERROR;
            return import_decl;
        }

        parser_advance(p);

        if (!parser_check(p, TOK_IDENTIFIER)) {
            SourceLocation location = source_location_from_token(
                p -> path,
                ctx -> buffer_start, 
                parser_peek(p) 
            );

            diag_error_help(
                ctx -> diag_ctx,
                location,
                "expected module name after '::'",
                "add a module name"
            );

            parser_recover_sync(p);
            import_decl -> kind = AST_ERROR;
            return import_decl;
        }
    }

    parser_advance(p);

    return import_decl;
}

AstNode* parse_enum_decl(MythrilContext* ctx, Parser* p) {
    AstNode* enum_decl = arena_alloc(p -> arena, sizeof(*enum_decl));

    return enum_decl;
}

AstNode* parse_struct_decl(MythrilContext* ctx, Parser* p) {
    AstNode* struct_decl = arena_alloc(p -> arena, sizeof(*struct_decl));

    return struct_decl;
}

AstNode* parse_impl_decl(MythrilContext* ctx, Parser* p) {
    AstNode* impl_decl = arena_alloc(p -> arena, sizeof(*impl_decl));

    return impl_decl;
}

AstNode* parse_function_decl(MythrilContext* ctx, Parser* p) {
    AstNode* function_decl = arena_alloc(p -> arena, sizeof(*function_decl));

    function_decl -> kind = AST_FUNCTION_DECL;
    function_decl -> function_decl.count = 0;

    parser_advance(p);

    if (!parser_check(p, TOK_IDENTIFIER)) {
        SourceLocation location = source_location_from_token(
            p -> path,
            ctx -> buffer_start, 
            parser_peek(p) 
        );

        location.length = 1;

        diag_error_help(
            ctx -> diag_ctx,
            location,
            "expected a function name",
            "try adding a name after 'fn' and before '('"
        );

        parser_recover_sync(p);

        function_decl -> kind = AST_ERROR;
        return function_decl;
    }

    Token* ident_token = parser_advance(p);

    AstSlice* slice = &function_decl -> function_decl.identifier;
    slice = ast_make_slice_from_token(p -> arena, ident_token);
    
    if (!parser_check(p, TOK_LEFT_PAREN)) {
        SourceLocation location = source_location_from_token(
            p -> path,
            ctx -> buffer_start, 
            parser_previous(p) 
        );

        location.column += location.length;
        location.pointer += location.length;
        location.length = 1;

        diag_error_help(
            ctx -> diag_ctx,
            location,
            "expected '(' after function name",
            "add '(' after function name"
        );

        parser_recover_sync(p);

        function_decl -> kind = AST_ERROR;
        return function_decl;
    }

    parser_advance(p);

    /* paramater parsing */
    while (!parser_check(p, TOK_RIGHT_PAREN)) {
        if (!parser_check(p, TOK_IDENTIFIER)) {
            SourceLocation location = source_location_from_token(
                p -> path,
                ctx -> buffer_start, 
                parser_previous(p) 
            );

            diag_error_help(
                ctx -> diag_ctx,
                location,
                "expected paramater name",
                "name your paramters"
            );

            parser_recover_sync(p);

            function_decl -> kind = AST_ERROR;
            return function_decl;
        }

        Token* param_name = parser_advance(p);

        if (!parser_check(p, TOK_COLON)) {
            SourceLocation location = source_location_from_token(
                p -> path,
                ctx -> buffer_start, 
                parser_previous(p) 
            );

            location.column += location.length;
            location.pointer += location.length;
            location.length = 1;

            diag_error_help(
                ctx -> diag_ctx,
                location,
                "expected ':' between parameter name and type",
                "add ':' here"
            );

            parser_recover_sync(p);

            function_decl -> kind = AST_ERROR;
            return function_decl;
        }
        
        parser_advance(p);

        Token* param_type = parser_peek(p);

        if (
            param_type -> kind != TOK_IDENTIFIER    && 
            param_type -> kind != TOK_VOID          && 
            !IS_PRIMITIVE_TYPE(param_type -> kind)
        ) {
            SourceLocation location = source_location_from_token(
                p -> path,
                ctx -> buffer_start, 
                parser_previous(p) 
            );

            diag_error_help(
                ctx -> diag_ctx,
                location,
                "expected parameter type",
                "add a type to parameter"
            );

            parser_recover_sync(p);

            function_decl -> kind = AST_ERROR;
            return function_decl;
        }

        function_decl -> function_decl.parameters = arena_alloc(p -> arena, sizeof(AstParameter) * 64);

        usize count = function_decl -> function_decl.count;
        AstParameter* param = &function_decl -> function_decl.parameters[count];

        param -> identifier = *ast_make_slice_from_token(p -> arena, param_name);
        param -> type = parse_type(ctx, p);

        function_decl -> function_decl.count++;

        if (parser_check(p, TOK_RIGHT_PAREN)) {
            break;
        }

        if (!parser_check(p, TOK_COMMA)) {
            SourceLocation location = source_location_from_token(
                p -> path,
                ctx -> buffer_start, 
                parser_previous(p) 
            );

            location.column += location.length;
            location.pointer += location.length;
            location.length = 1;

            diag_error_help(
                ctx -> diag_ctx,
                location,
                "expected a ',' between parameters",
                "add a ',' here"
            );

            parser_recover_sync(p);

            function_decl -> kind = AST_ERROR;
            return function_decl;
        }

        parser_advance(p);
    } /* end of parameters */

    parser_advance(p);

    if (!parser_check(p, TOK_COLON)) {
        SourceLocation location = source_location_from_token(
            p -> path,
            ctx -> buffer_start, 
            parser_previous(p) 
        );

        location.column += location.length;
        location.pointer += location.length;
        location.length = 1;

        diag_error_help(
            ctx -> diag_ctx,
            location,
            "expected a ':' after parameters, before return type",
            "add a ':' here"
        );

        parser_recover_sync(p);

        function_decl -> kind = AST_ERROR;
        return function_decl;
    }

    parser_advance(p);

    Token* return_type = parser_peek(p);

    if (
        return_type -> kind != TOK_IDENTIFIER   && 
        return_type -> kind != TOK_VOID         &&
        !IS_PRIMITIVE_TYPE(return_type -> kind)
    ) {
        SourceLocation location = source_location_from_token(
            p -> path,
            ctx -> buffer_start, 
            parser_previous(p) 
        );

        location.column += location.length;
        location.pointer += location.length;
        location.length = 1;

        diag_error_help(
            ctx -> diag_ctx,
            location,
            "expected return type",
            "add a return type to function"
        );

        parser_recover_sync(p);

        function_decl -> kind = AST_ERROR;
        return function_decl;
    }

    function_decl -> function_decl.return_type = parse_type(ctx, p);

    if (!parser_check(p, TOK_LEFT_BRACE)) {
        SourceLocation location = source_location_from_token(
            p -> path,
            ctx -> buffer_start, 
            parser_previous(p) 
        );

        location.column += location.length;
        location.pointer += location.length;
        location.length = 1;

        diag_error_help(
            ctx -> diag_ctx,
            location,
            "expected a '{' to mark start of block",
            "add a '{' here"
        );

        parser_recover_sync(p);

        function_decl -> kind = AST_ERROR;
        return function_decl;
    }

    parser_advance(p);

    function_decl -> function_decl.block = parse_block(ctx, p);

    if (!parser_check(p, TOK_RIGHT_BRACE)) {
        SourceLocation location = source_location_from_token(
            p -> path,
            ctx -> buffer_start, 
            parser_previous(p) 
        );

        location.column += location.length;
        location.pointer += location.length;
        location.length = 1;

        diag_error_help(
            ctx -> diag_ctx,
            location,
            "expected a '}' to mark end of block",
            "add a '}' here"
        );

        parser_recover_sync(p);

        function_decl -> kind = AST_ERROR;
        return function_decl;
    }
    
    parser_advance(p);

    return function_decl;
}

AstNode* parse_const_decl(MythrilContext* ctx, Parser* p) {
    AstNode* const_decl = arena_alloc(p -> arena, sizeof(*const_decl));

    return const_decl;
}

AstNode* parse_static_decl(MythrilContext* ctx, Parser* p) {
    AstNode* static_decl = arena_alloc(p -> arena, sizeof(*static_decl));

    return static_decl;
}

AstVec parse_block(MythrilContext* ctx, Parser* p) {
    parser_advance(p);

    AstVec vec = {
        .items = arena_alloc(p -> arena, sizeof(AstNode*) * 8),
        .count = 0,
        .capacity = 8
    };

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
        // ast_warn(parser_peek(p), p, "Empty block");
    }

    return vec;

}

AstNode* parse_statement(MythrilContext* ctx, Parser* p) {

}

AstNode* parse_expression(MythrilContext* ctx, Parser* p) {
    AstNode* expression = arena_alloc(p -> arena, sizeof(*expression));

    return expression;
}

AstType* parse_type(MythrilContext* ctx, Parser* p) {
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
                    SourceLocation location = source_location_from_token(
                        p -> path,
                        ctx -> buffer_start,
                        parser_peek(p)
                    );
                    diag_error_help(
                        ctx -> diag_ctx,
                        location,
                        "expected array size expression or ']'",
                        "provide a constant expression like '10' or 'SIZE_CONST'"
                    );
                    parser_recover_sync(p);
                    return NULL;
                }
            } else {
                array_type -> array.size_expr = NULL;
            }
            
            if (!parser_check(p, TOK_RIGHT_SQUARE)) {
                SourceLocation location = source_location_from_token(
                    p -> path,
                    ctx -> buffer_start,
                    parser_peek(p)
                );

                diag_error_help(
                    ctx -> diag_ctx,
                    location,
                    "expected ']' after array size",
                    "add ']' to close the array type declaration"
                );

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
