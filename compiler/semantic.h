#ifndef SEMANTIC_H
#define SEMANTIC_H

#include "ast.h"

void semantic_analysis(ASTNode* node);
void semantic_errors_to_json();
extern void add_error(const char* msg, int line); // using the same error list from parser

#endif
