#include "diagnostics.h"
#include "types.h"

#include "../utils/ansi_codes.h"

#include <stdio.h>
#include <stdarg.h>
#include <string.h>

void add_diagnostic(
    DiagContext* ctx,
    DiagnosticLevel level,
    SourceLocation location,
    const char* msg,
    const char* help
) {
    if (level == DIAGNOSTIC_WARN) {
        ctx -> warning_count++;
    } else if (level == DIAGNOSTIC_ERROR) {
        ctx -> error_count++;
    }

    u32 total = ctx -> warning_count + ctx -> error_count;

    if (total >= MAX_DIAGNOSITCS) {
        return;
    }

    Diagnostic* diag = &ctx -> nodes[ctx -> index++];

    diag -> level = level;
    diag -> location = location;
    diag -> message = msg;
    diag -> help = help;
}

char* format_message(ArenaAllocator* arena, const char* fmt, va_list va_args) {
    va_list args;

    va_copy(args, va_args);
    i32 size = vsnprintf(NULL, 0, fmt, args);
    va_end(args);

    if (size < 0) {
        return nullptr;
    }

    char* buffer = arena_alloc(arena, size + 1);
    vsnprintf(buffer, size + 1, fmt, va_args);

    return buffer;
}

void diag_error(DiagContext* ctx, SourceLocation loc, const char* fmt, ...) {
    va_list args;

    va_start(args, fmt);
    char* message = format_message(ctx -> arena, fmt, args);
    va_end(args);
    
    add_diagnostic(ctx, DIAGNOSTIC_ERROR, loc, message, NULL);
}

void diag_warning(DiagContext* ctx, SourceLocation loc, const char* fmt, ...) {
    va_list args;

    va_start(args, fmt);
    char* message = format_message(ctx -> arena, fmt, args);
    va_end(args);
    
    add_diagnostic(ctx, DIAGNOSTIC_WARN, loc, message, NULL);
}

void diag_note(DiagContext* ctx, SourceLocation loc, const char* fmt, ...) {
    va_list args;

    va_start(args, fmt);
    char* message = format_message(ctx -> arena, fmt, args);
    va_end(args);
    
    add_diagnostic(ctx, DIAGNOSTIC_NOTE, loc, message, NULL);
}

void diag_error_help(DiagContext* ctx, SourceLocation loc, const char* message, const char* help) {
    usize msg_len = strlen(message);
    usize help_len = strlen(help);
    
    char* msg_copy = arena_alloc(ctx -> arena, msg_len + 1);
    char* help_copy = arena_alloc(ctx -> arena, help_len + 1);
    
    arena_memcpy(msg_copy, message, msg_len + 1);
    arena_memcpy(help_copy, help, help_len + 1);
    
    add_diagnostic(ctx, DIAGNOSTIC_ERROR, loc, msg_copy, help_copy);
}

/*
*   nice premade helper things
*/

void diag_expected(DiagContext* ctx, SourceLocation loc, const char* expected) {
    char* msg = arena_alloc(ctx -> arena, 256);

    snprintf(msg, 256, "expected %s", expected);

    add_diagnostic(ctx, DIAGNOSTIC_ERROR, loc, msg, NULL);
}

void diag_expected_found(DiagContext* ctx, SourceLocation loc, const char* expected, const char* found) {
    char* msg = arena_alloc(ctx -> arena, 512);

    snprintf(msg, 512, "expected %s, found %s", expected, found);

    add_diagnostic(ctx, DIAGNOSTIC_ERROR, loc, msg, NULL);
}

void diag_undefined(DiagContext* ctx, SourceLocation loc, const char* what, const char* name) {
    char* msg = arena_alloc(ctx -> arena, 512);

    snprintf(msg, 512, "undefined %s '%s'", what, name);

    add_diagnostic(ctx, DIAGNOSTIC_ERROR, loc, msg, NULL);
}

void diag_redefined(DiagContext* ctx, SourceLocation loc, const char* type, const char* name) {
    char* msg = arena_alloc(ctx -> arena, 512);

    snprintf(msg, 512, "%s '%s' is already defined", type, name);

    add_diagnostic(ctx, DIAGNOSTIC_ERROR, loc, msg, NULL);
}
