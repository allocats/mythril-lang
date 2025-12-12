#include "errors.h"

#include "../../diagnostics/diagnostics.h"

#include <ctype.h>

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

void error_whole_line(MythrilContext* ctx, Parser* p, const char* msg, const char* help) {
    Token* line = parser_peek(p);

    const char* start = line -> lexeme;
    const char* end = line -> lexeme;

    const char* ptr_copy = line -> lexeme;
    const usize len_copy = line -> length;  

    while (*end != '\n') {
        end++;
    }

    while (*(start - 1) != '\n') {
        start--;
    }

    line -> lexeme = start;
    line -> length = end - start;

    SourceLocation location = source_location_from_token(
        p -> path,
        ctx -> buffer_start,
        line
    );

    diag_error_help(ctx -> diag_ctx, location, msg, help);

    line -> lexeme = ptr_copy; 
    line -> length = len_copy;
}
