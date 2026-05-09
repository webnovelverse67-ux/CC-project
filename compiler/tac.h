#ifndef TAC_H
#define TAC_H

#include "ast.h"

typedef struct TAC {
    char* op;
    char* arg1;
    char* arg2;
    char* res;
    struct TAC* next;
} TAC;

TAC* create_tac(char* op, char* arg1, char* arg2, char* res);
void append_tac(TAC* tac);
char* new_temp();
char* new_label();

void generate_tac(ASTNode* node);
void tac_to_json();

#endif
