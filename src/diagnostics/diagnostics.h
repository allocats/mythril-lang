#pragma once
#ifndef MYTHRIL_DIAGNOSTICS_H
#define MYTHRIL_DIAGNOSTICS_H

#include "types.h"

#include "../tokens/types.h"

#include <stdarg.h>

/*
*
*   adds a diagnostic to the array, no more than 32 diagnostics 
*   will be in the array. Warning/Error counts continue to 
*   increment regardless though, this is just to limit the output
*   to MAX_DIAGNOSTICS (defined in types.h)
*
*/
void add_diagnostic(
    DiagContext* ctx,
    DiagnosticLevel level,
    SourceLocation location,
    const char* msg,
    const char* help
);

/*
*
*   simply formats the message based on the parameters
*   fmt, va_args and allocates the message on the arena
*   it's a C string, null terminated
*
*/
char* format_message(ArenaAllocator* arena, const char* fmt, va_list va_args);

/*
*
*   Add an error to the array
*
*/
void diag_error(DiagContext* ctx, SourceLocation loc, const char* fmt, ...);

/*
*
*   Add a warning to the array
*
*/
void diag_warning(DiagContext* ctx, SourceLocation loc, const char* fmt, ...);

/*
*
*   Add a note to the array
*
*/
void diag_note(DiagContext* ctx, SourceLocation loc, const char* fmt, ...);


/*
*
*   Add error message, with a help/potential fix
*
*/
void diag_error_help(DiagContext* ctx, SourceLocation loc, const char* message, const char* help);

/*
*
*   Premade expected error
*
*/
void diag_expected(DiagContext* ctx, SourceLocation loc, const char* expected, const char* help);

/*
*
*   Premade expected, but found error
*
*/
void diag_expected_found(DiagContext* ctx, SourceLocation loc, const char* expected, const char* found);

/*
*
*   Premade undefined error
*
*/
void diag_undefined(
    DiagContext* ctx,
    SourceLocation loc,
    const char* what,
    const char* name,
    const char* help
);

/*
*
*   Premade redefined error
*
*/
void diag_redefined(DiagContext* ctx, SourceLocation loc, const char* type, const char* name);

/*
*
*   get the location within the source buffer from a token
*
*/
SourceLocation source_location_from_token(const char* path, const char* source, Token* token);

/*
*
*   get the full line of code where the error is present
*
*/
void get_source_line(const char* source, const char* ptr, const char** line_start, usize* line_len);

/*
*
*   get ansi escape code sequence for each level
*
*/
const char* level_colour(DiagnosticLevel level);

/*
*
*   get each level as strings
*
*/
const char* level_string(DiagnosticLevel level);

/*
*
*   print single diagnostic
*
*/
void diagnostics_print(DiagContext* ctx, Diagnostic* diag);

/*
*
*   print all diagnostic
*
*/
void diagnostics_print_all(DiagContext* ctx);

#endif // !MYTHRIL_DIAGNOSTICS_H
