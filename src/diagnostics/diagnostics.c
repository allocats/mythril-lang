#include "diagnostics.h"
#include "types.h"

#include "../mythril/types.h"
#include "../tokens/types.h"
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
    } else if (level == DIAGNOSTIC_NOTE) {
        ctx -> note_count++;
    }

    u32 total = ctx -> warning_count + ctx -> error_count + ctx -> note_count;

    if (total >= MAX_DIAGNOSTICS) {
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

void diag_expected(DiagContext* ctx, SourceLocation loc, const char* expected, const char* help) {
    char* msg = arena_alloc(ctx -> arena, 256);

    snprintf(msg, 256, "expected %s", expected);

    add_diagnostic(ctx, DIAGNOSTIC_ERROR, loc, msg, help);
}

void diag_expected_found(DiagContext* ctx, SourceLocation loc, const char* expected, const char* found) {
    char* msg = arena_alloc(ctx -> arena, 512);

    snprintf(msg, 512, "expected %s, found %s", expected, found);

    add_diagnostic(ctx, DIAGNOSTIC_ERROR, loc, msg, NULL);
}

void diag_undefined(
    DiagContext* ctx,
    SourceLocation loc,
    const char* what,
    const char* name,
    const char* help
) {
    char* msg = arena_alloc(ctx -> arena, 512);

    snprintf(msg, 512, "undefined %s '%s'", what, name);

    add_diagnostic(ctx, DIAGNOSTIC_ERROR, loc, msg, help);
}

void diag_redefined(DiagContext* ctx, SourceLocation loc, const char* type, const char* name) {
    char* msg = arena_alloc(ctx -> arena, 512);

    snprintf(msg, 512, "%s '%s' is already defined", type, name);

    add_diagnostic(ctx, DIAGNOSTIC_ERROR, loc, msg, NULL);
}

SourceLocation source_location_from_token(const char* path, const char* source, Token* token) {
    usize line = 1;
    usize column = 1;

    const char* p = source;

    while (p < token -> lexeme) {
        if (*p == '\n') {
            line++;
            column = 1;
        } else {
            column++;
        }

        p++;
    }

    return (SourceLocation) {
        .path = path,
        .source_buffer = source,
        .line = line,
        .column = column,
        .pointer = token -> lexeme,
        .length = token -> length
    };
}

void get_source_line(const char* source, const char* ptr, const char** line_start, usize* line_len) {
    const char* start = ptr;
    const char* end = ptr;

    while (start > source && *(start - 1) != '\n') {
        start--;
    }

    while (*end != 0 && *(end) != '\n') {
        end++;
    }

    *line_start = start;
    *line_len   = end - start;
}

const char* level_colour(DiagnosticLevel level) {
    switch (level) {
        case DIAGNOSTIC_ERROR:  return ANSI_RED;
        case DIAGNOSTIC_WARN:   return ANSI_YELLOW;
        case DIAGNOSTIC_NOTE:   return ANSI_MAGENTA;
    }
}
const char* level_strings(DiagnosticLevel level) {
    switch (level) {
        case DIAGNOSTIC_ERROR:  return "error";
        case DIAGNOSTIC_WARN:   return "warn";
        case DIAGNOSTIC_NOTE:   return "note";
    }
}

void diagnostics_print(DiagContext* ctx, Diagnostic* diag) {
    b8 colour_support = ctx -> stderr_supports_colours;

    const char* colour = colour_support ? level_colour(diag -> level) : "";
    const char* reset  = colour_support ? ANSI_RESET : "";
    const char* bold   = colour_support ? ANSI_BOLD : "";

    const char* green   = colour_support ? ANSI_GREEN : "";
    const char* magenta = colour_support ? ANSI_MAGENTA : "";
    
    // header
    fprintf(
        stderr,
        "%s%s%s%s: %s%s%s\n",
        bold,
        colour,
        level_strings(diag -> level),
        reset,
        bold,
        diag -> message,
        reset
    );
    
    // location
    fprintf(
        stderr,
        " %s%s-->%s %s:%zu:%zu\n",
        magenta,
        bold,
        reset,
        diag -> location.path,
        diag -> location.line,
        diag -> location.column
    );
    
    // source line
    const char* line_start;
    usize line_len;

    get_source_line(diag -> location.source_buffer, diag -> location.pointer, &line_start, &line_len);
    
    // source context
    fprintf(stderr, "  %s|%s\n", bold, reset);
    fprintf(stderr, "%zu %s|%s ", diag -> location.line, bold, reset);
    fprintf(stderr, "%.*s\n", (i32) line_len, line_start);
    fprintf(stderr, "  %s|%s ", bold, reset);
    
    usize spaces = diag -> location.column - 1;

    for (usize i = 0; i < spaces; i++) {
        fprintf(stderr, " ");
    }
    
    fprintf(stderr, "%s%s", green, bold);

    usize caret_len = diag -> location.length > 0 ? diag -> location.length : 1;

    for (usize i = 0; i < caret_len; i++) {
        fprintf(stderr, "^");
    }

    fprintf(stderr, "%s", reset);
    
    if (diag -> help) {
        fprintf(stderr, " %s%shelp: %s%s", bold, green, diag -> help, reset);
    }
    
    fprintf(stderr, "\n");
    fprintf(stderr, "  %s|%s\n", bold, reset);
}

void diagnostics_print_all(DiagContext* ctx) {
    b8 colour_support = ctx -> stderr_supports_colours;
    
    for (u32 i = 0; i < ctx -> index; i++) {
        diagnostics_print(ctx, &ctx -> nodes[i]);
        fprintf(stderr, "\n");
    }
    
    usize total_count = ctx -> error_count + ctx -> warning_count + ctx -> note_count;

    if (total_count > MAX_DIAGNOSTICS) {
        fprintf(
            stderr,
            "... and %zu more diagnostic%s not shown\n\n",
            total_count - MAX_DIAGNOSTICS,
            (total_count - MAX_DIAGNOSTICS) == 1 ? "" : "s"
        );
    }
    
    if (ctx -> error_count > 0 || ctx -> warning_count > 0) {
        fprintf(
            stderr,
            "compilation %sfailed%s due to %u error%s",
            colour_support ? ANSI_RED : "",
            colour_support ? ANSI_RESET : "",
            ctx -> error_count,
            ctx -> error_count == 1 ? "" : "s"
        );
        
        if (ctx -> warning_count > 0) {
            fprintf(
                stderr,
                " and %u warning%s",
                ctx -> warning_count,
                ctx -> warning_count == 1 ? "" : "s"
            );
        }
        
        fprintf(stderr, "%s\n", colour_support ? ANSI_RESET : "");
    }
}
