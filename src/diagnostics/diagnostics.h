#pragma once
#ifndef MYTHRIL_DIAGNOSTICS_H
#define MYTHRIL_DIAGNOSTICS_H

#include "types.h"

void add_diagnostic(
    DiagContext* ctx,
    DiagnosticLevel level,
    SourceLocation location,
    const char* msg,
    const char* help
);

#endif // !MYTHRIL_DIAGNOSTICS_H
