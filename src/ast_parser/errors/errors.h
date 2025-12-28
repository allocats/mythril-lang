#pragma once
#ifndef MYTHRIL_AST_PARSER_ERRORS_H
#define MYTHRIL_AST_PARSER_ERRORS_H

#include "../parser.h"

void error_at_current(MythrilContext* ctx, Parser* p, const char* msg, const char* help);
void error_at_previous(MythrilContext* ctx, Parser* p, const char* msg, const char* help);
void error_at_previous_end(MythrilContext* ctx, Parser* p, const char* msg, const char* help);
void error_till_end_of_line(MythrilContext* ctx, Parser* p, const char* msg, const char* help);
void error_whole_line(MythrilContext* ctx, Parser* p, const char* msg, const char* help);

#endif // !MYTHRIL_
