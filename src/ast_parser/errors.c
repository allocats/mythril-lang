#include "parser.h"

void recover_to_top_level_decl(Parser* p) {
    TokenKind kind = parser_peek(p) -> kind;

    while (
        kind != TOK_MODULE    &&
        kind != TOK_IMPORT    &&
        kind != TOK_STRUCT    &&
        kind != TOK_ENUM      &&
        kind != TOK_IMPL      &&
        kind != TOK_FUNCTION  &&
        kind != TOK_STATIC    &&
        kind != TOK_CONST     &&
        kind != TOK_EOF       &&
        kind != TOK_EOP
    ) {
        parser_advance(p);
        
        kind = parser_peek(p) -> kind;
    }
}

void recover_in_path_segment(Parser* p) {
    TokenKind kind = parser_peek(p) -> kind;

    while (
        kind != TOK_IDENTIFIER  &&
        kind != TOK_COLON_COLON &&
        kind != TOK_SEMI_COLON  &&
        kind != TOK_EOF         &&
        kind != TOK_EOP
    ) {
        parser_advance(p);

        kind = parser_peek(p) -> kind;
    }
}

void recover_in_fn_decl(Parser* p) {
    TokenKind kind = parser_peek(p) -> kind;

    while (
        kind != TOK_IDENTIFIER  &&
        kind != TOK_COLON_COLON &&
        kind != TOK_SEMI_COLON  &&
        kind != TOK_EOF         &&
        kind != TOK_EOP
    ) {
        parser_advance(p);

        kind = parser_peek(p) -> kind;
    }
}
