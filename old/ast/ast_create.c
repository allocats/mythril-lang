#include "ast.h"
#include "types.h"

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

    node -> ast.function.name_ptr = name_ptr; 
    node -> ast.function.name_len = name_len;

    node -> ast.function.ret_ptr = ret_ptr;
    node -> ast.function.ret_len = ret_len;

    node -> ast.function.params = params;
    node -> ast.function.param_count = param_count;

    node -> ast.function.block = block;

    return node;
}

AstNode* ast_create_var_decl(
    ArenaAllocator* arena,
    char* type_ptr,
    usize type_len,
    u32 pointer_depth,
    char* name_ptr,
    usize name_len,
    AstNode* initializer 
) {
    AstNode* node = arena_alloc(arena, sizeof(*node));

    node -> type = A_VAR_DECL;

    node -> ast.var_decl.type_ptr = type_ptr;
    node -> ast.var_decl.type_len = type_len;

    node -> ast.var_decl.pointer_depth = pointer_depth;

    node -> ast.var_decl.name_ptr = name_ptr;
    node -> ast.var_decl.name_len = name_len;

    node -> ast.var_decl.initializer = initializer;

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

    node -> ast.function_call.name_ptr = name_ptr;
    node -> ast.function_call.name_len = name_len;

    node -> ast.function_call.args = args;
    node -> ast.function_call.arg_count = arg_count;

    return node;
}

AstNode* ast_create_block(
    ArenaAllocator* arena,
    AstNode** statements,
    usize count
) {
    AstNode* node = arena_alloc(arena, sizeof(*node));

    node -> type = A_BLOCK;

    node -> ast.block.statements = statements;
    node -> ast.block.count = count;

    return node;
}

AstNode* ast_create_identifier(
    ArenaAllocator* arena,
    char* name_ptr,
    usize name_len
) {
    AstNode* node = arena_alloc(arena, sizeof(*node));

    node -> type = A_IDENTIFIER;

    node -> ast.identifier.name_ptr = name_ptr;
    node -> ast.identifier.name_len = name_len;

    return node;
}

AstNode* ast_create_number_literal(
    ArenaAllocator* arena,
    u64 value 
) {
    AstNode* node = arena_alloc(arena, sizeof(*node));

    node -> type = A_LITERAL;

    node -> ast.literal.type = T_NUMBER;
    node -> ast.literal.value.int_value = value;

    return node;
}

AstNode* ast_create_string_literal(
    ArenaAllocator* arena,
    char* ptr,
    u32 len
) {
    AstNode* node = arena_alloc(arena, sizeof(*node));

    node -> type = A_LITERAL;

    node -> ast.literal.type = T_STR_LITERAL;
    node -> ast.literal.value.string.ptr = ptr;
    node -> ast.literal.value.string.len = len;

    return node;
}

AstNode* ast_create_return(
    ArenaAllocator* arena,
    AstNode* expr
) {
    AstNode* node = arena_alloc(arena, sizeof(*node));

    node -> type = A_RETURN;
    node -> ast.ret.expression = expr;

    return node;
}

AstNode* ast_create_assignment(
    ArenaAllocator* arena,
    AstNode* target,
    AstNode* value 
) {
    AstNode* node = arena_alloc(arena, sizeof(*node));

    node -> type = A_ASSIGNMENT;

    node -> ast.assignment.target = target;
    node -> ast.assignment.value = value;

    return node;
}


AstNode* ast_create_binary_expr(
    ArenaAllocator* arena,
    TokenType op_type,
    AstNode* left,
    AstNode* right
) {
    AstNode* node = arena_alloc(arena, sizeof(*node));

    node -> type = A_EXPR;

    node -> ast.expression.kind = EXPR_BINARY;
    node -> ast.expression.expr.binary.op_kind = op_type;
    node -> ast.expression.expr.binary.left = left;
    node -> ast.expression.expr.binary.right = right;

    return node;
}
