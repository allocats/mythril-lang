#include "ast.h"
#include "types.h"

[[gnu::always_inline]]
b32 ast_is_decl(AstNode* node) {
    return node -> kind >= AST_IMPORT && node -> kind <= AST_STATIC_VAR;
}

[[gnu::always_inline]]
b32 ast_is_stmt(AstNode* node) {
    return node -> kind >= AST_VAR_DECL && node -> kind <= AST_RETURN;
}

[[gnu::always_inline]]
b32 ast_is_expr(AstNode* node) {
    return node -> kind >= AST_UNARY && node -> kind <= AST_IDENTIFIER;
}

/*
*
*   Build the AST as well as perform semantic analysis,
*   nodes marked as 'check again' are walked again after
*
*/
void ast_build(ArenaAllocator* arena, Tokens* tokens) {

}
