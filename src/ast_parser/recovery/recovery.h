#pragma once
#ifndef MYTHRIL_AST_PARSER_RECOVERY_H
#define MYTHRIL_AST_PARSER_RECOVERY_H

#include "../parser.h"

void recover_to_top_level_decl(Parser* p);
void recover_in_path_segment(Parser* p);
void recover_in_fn_params(Parser* p);

#endif // !MYTHRIL_AST_PARSER_RECOVERY_H
