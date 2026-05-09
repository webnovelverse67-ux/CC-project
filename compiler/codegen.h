#ifndef CODEGEN_H
#define CODEGEN_H

#include "ast.h"

void generate_target_code(ASTNode* node, const char* lang, const char* out_file_path);

#endif
