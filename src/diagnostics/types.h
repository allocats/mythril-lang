#pragma once
#ifndef MYTHRIL_DIAGNOSTICS_TYPES_H
#define MYTHRIL_DIAGNOSTICS_TYPES_H

#include "../arena/arena.h"
#include "../utils/types.h"

#define MAX_DIAGNOSTICS 32

typedef enum {
    DIAGNOSTIC_ERROR,
    DIAGNOSTIC_WARN,
    DIAGNOSTIC_NOTE
} DiagnosticLevel;
 
typedef struct {
    const char* path; 
    const char* source_buffer;

    usize line;
    usize column;

    const char* pointer; // start of problematic code
    usize length;
} SourceLocation;

typedef struct {
    DiagnosticLevel level;
    SourceLocation  location;

    const char* message;    // COMPLETE formatted message
    const char* help;       // suggested fix 
} Diagnostic;

typedef struct {
    ArenaAllocator* arena;
    const char* path;
    const char* source_buffer;

    Diagnostic nodes[MAX_DIAGNOSTICS];
    u32 index;

    u32 warning_count;
    u32 error_count;
    u32 note_count;

    b8 stdout_supports_colours;
} DiagContext;

#endif // !MYTHRIL_DIAGNOSTICS_TYPES_H
