#include "parser.h"
#include "types.h"

#include "precedence/precedence.h"

AstNode* parse_module_decl(MythrilContext* ctx, Parser* p) {
    AstNode* node = arena_alloc(p -> arena, sizeof(*node));

    return node;
}

AstNode* parse_import_decl(MythrilContext* ctx, Parser* p) {
    AstNode* node = arena_alloc(p -> arena, sizeof(*node));

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

AstNode* parse_impl_decl(MythrilContext* ctx, Parser* p) {
    AstNode* node = arena_alloc(p -> arena, sizeof(*node));

    return node;
}

AstNode* parse_function_decl(MythrilContext* ctx, Parser* p) {
    AstNode* node = arena_alloc(p -> arena, sizeof(*node));

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

AstType* parse_type(MythrilContext* ctx, Parser* p);

Token* parser_peek(Parser* p) {
    if (p -> index >= p -> count) {
        return &p -> tokens -> items[p -> count];
    }

    return &p -> tokens -> items[p -> index];
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
