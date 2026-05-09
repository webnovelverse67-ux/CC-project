%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"

extern int yylex();
extern int yylineno;
extern char* yytext;
void yyerror(const char *s);

ASTNode* root = NULL;

// Store errors for JSON output
typedef struct ErrorList {
    char* message;
    int line;
    struct ErrorList* next;
} ErrorList;

ErrorList* errors_head = NULL;
ErrorList* errors_tail = NULL;

void add_error(const char* msg, int line) {
    ErrorList* err = (ErrorList*)malloc(sizeof(ErrorList));
    err->message = strdup(msg);
    err->line = line;
    err->next = NULL;
    if (!errors_head) {
        errors_head = err;
        errors_tail = err;
    } else {
        errors_tail->next = err;
        errors_tail = err;
    }
}

void yyerror(const char *s) {
    char buf[256];
    snprintf(buf, sizeof(buf), "Syntax Error: %s at '%s'", s, yytext);
    add_error(buf, yylineno);
}

void errors_to_json() {
    printf("[\n");
    ErrorList* curr = errors_head;
    int first = 1;
    while(curr) {
        if(!first) printf(",\n");
        first = 0;
        printf("  {\"message\": \"%s\", \"line\": %d}", curr->message, curr->line);
        curr = curr->next;
    }
    printf("\n]");
}

%}

%union {
    int ival;
    float fval;
    char* strval;
    struct ASTNode* node;
}

%token INT FLOAT IF ELSE WHILE PRINT
%token <strval> ID
%token <ival> INT_CONST
%token <fval> FLOAT_CONST
%token PLUS MINUS MUL DIV ASSIGN EQ NEQ LT LTE GT GTE
%token LBRACE RBRACE LPAREN RPAREN SEMI COMMA

%type <node> program stmt_list stmt var_decl expr assign_stmt if_stmt while_stmt print_stmt block

%left EQ NEQ LT LTE GT GTE
%left PLUS MINUS
%left MUL DIV

%%
program:
    stmt_list {
        root = create_node(AST_PROGRAM, $1, NULL);
    }
    ;

stmt_list:
    stmt_list stmt { $$ = append_sibling($1, $2); }
    | stmt { $$ = $1; }
    ;

stmt:
    var_decl SEMI { $$ = $1; }
    | assign_stmt SEMI { $$ = $1; }
    | if_stmt { $$ = $1; }
    | while_stmt { $$ = $1; }
    | print_stmt SEMI { $$ = $1; }
    | block { $$ = $1; }
    | error SEMI { $$ = NULL; /* error recovery */ }
    ;

block:
    LBRACE stmt_list RBRACE { $$ = create_node(AST_BLOCK, $2, NULL); }
    | LBRACE RBRACE { $$ = create_node(AST_BLOCK, NULL, NULL); }
    ;

var_decl:
    INT ID {
        ASTNode* var = create_var($2);
        var->data_type = strdup("int");
        $$ = create_node(AST_VAR_DECL, var, NULL);
    }
    | FLOAT ID {
        ASTNode* var = create_var($2);
        var->data_type = strdup("float");
        $$ = create_node(AST_VAR_DECL, var, NULL);
    }
    | INT ID ASSIGN expr {
        ASTNode* var = create_var($2);
        var->data_type = strdup("int");
        $$ = create_node(AST_VAR_DECL, var, $4);
    }
    | FLOAT ID ASSIGN expr {
        ASTNode* var = create_var($2);
        var->data_type = strdup("float");
        $$ = create_node(AST_VAR_DECL, var, $4);
    }
    ;

assign_stmt:
    ID ASSIGN expr {
        $$ = create_node(AST_ASSIGN, create_var($1), $3);
    }
    ;

if_stmt:
    IF LPAREN expr RPAREN stmt {
        $$ = create_node(AST_IF, $3, $5);
    }
    | IF LPAREN expr RPAREN stmt ELSE stmt {
        ASTNode* right = $5;
        right = append_sibling(right, $7);
        $$ = create_node(AST_IF, $3, right);
    }
    ;

while_stmt:
    WHILE LPAREN expr RPAREN stmt {
        $$ = create_node(AST_WHILE, $3, $5);
    }
    ;

print_stmt:
    PRINT LPAREN expr RPAREN {
        $$ = create_node(AST_PRINT, $3, NULL);
    }
    ;

expr:
    expr PLUS expr { $$ = create_binop("+", $1, $3); }
    | expr MINUS expr { $$ = create_binop("-", $1, $3); }
    | expr MUL expr { $$ = create_binop("*", $1, $3); }
    | expr DIV expr { $$ = create_binop("/", $1, $3); }
    | expr EQ expr { $$ = create_binop("==", $1, $3); }
    | expr NEQ expr { $$ = create_binop("!=", $1, $3); }
    | expr LT expr { $$ = create_binop("<", $1, $3); }
    | expr LTE expr { $$ = create_binop("<=", $1, $3); }
    | expr GT expr { $$ = create_binop(">", $1, $3); }
    | expr GTE expr { $$ = create_binop(">=", $1, $3); }
    | LPAREN expr RPAREN { $$ = $2; }
    | ID { $$ = create_var($1); }
    | INT_CONST { $$ = create_literal_int($1); }
    | FLOAT_CONST { $$ = create_literal_float($1); }
    ;

%%
