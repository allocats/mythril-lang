#include "parser.h"

#include "../diagnostics/diagnostics.h"

void error_at_current(MythrilContext* ctx, Parser* p, const char* msg, const char* help) {
    SourceLocation location = source_location_from_token(
        p -> path,
        ctx -> buffer_start,
        parser_peek(p)
    );

    diag_error_help(ctx -> diag_ctx, location, msg, help);
}

void error_at_previous(MythrilContext* ctx, Parser* p, const char* msg, const char* help) {
    SourceLocation location = source_location_from_token(
        p -> path,
        ctx -> buffer_start,
        parser_peek_previous(p)
    );

    diag_error_help(ctx -> diag_ctx, location, msg, help);
}

void error_at_previous_end(MythrilContext* ctx, Parser* p, const char* msg, const char* help) {
    Token* token = parser_peek_previous(p);

    token -> lexeme += token -> length;
    token -> length = 1;

    SourceLocation location = source_location_from_token(
        p -> path,
        ctx -> buffer_start,
        token
    );

    diag_error_help(ctx -> diag_ctx, location, msg, help);
}

void error_till_end_of_line(MythrilContext* ctx, Parser* p, const char* msg, const char* help) {
    Token* line = parser_peek(p);

    const char* cursor = line -> lexeme;

    while (*cursor != '\n') {
        cursor++;
    }

    line -> length = cursor - line -> lexeme;

    SourceLocation location = source_location_from_token(
        p -> path,
        ctx -> buffer_start,
        line
    );

    diag_error_help(ctx -> diag_ctx, location, msg, help);
}
