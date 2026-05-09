%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"
#include "parser.tab.h"

// For JSON token output
typedef struct TokenList {
    char* type;
    char* value;
    int line;
    struct TokenList* next;
} TokenList;

TokenList* tokens_head = NULL;
TokenList* tokens_tail = NULL;

void add_token(const char* type, const char* value, int line) {
    TokenList* tok = (TokenList*)malloc(sizeof(TokenList));
    tok->type = strdup(type);
    tok->value = strdup(value);
    tok->line = line;
    tok->next = NULL;

    if (!tokens_head) {
        tokens_head = tok;
        tokens_tail = tok;
    } else {
        tokens_tail->next = tok;
        tokens_tail = tok;
    }
}

%}

%option noyywrap
%option yylineno

%%
"int"       { add_token("KEYWORD", yytext, yylineno); return INT; }
"float"     { add_token("KEYWORD", yytext, yylineno); return FLOAT; }
"if"        { add_token("KEYWORD", yytext, yylineno); return IF; }
"else"      { add_token("KEYWORD", yytext, yylineno); return ELSE; }
"while"     { add_token("KEYWORD", yytext, yylineno); return WHILE; }
"print"     { add_token("KEYWORD", yytext, yylineno); return PRINT; }

[a-zA-Z_][a-zA-Z0-9_]* {
    add_token("IDENTIFIER", yytext, yylineno);
    yylval.strval = strdup(yytext);
    return ID;
}

[0-9]+\.[0-9]+ {
    add_token("FLOAT_LIT", yytext, yylineno);
    yylval.fval = atof(yytext);
    return FLOAT_CONST;
}

[0-9]+      {
    add_token("INT_LIT", yytext, yylineno);
    yylval.ival = atoi(yytext);
    return INT_CONST;
}

"+"         { add_token("OPERATOR", yytext, yylineno); return PLUS; }
"-"         { add_token("OPERATOR", yytext, yylineno); return MINUS; }
"*"         { add_token("OPERATOR", yytext, yylineno); return MUL; }
"/"         { add_token("OPERATOR", yytext, yylineno); return DIV; }
"="         { add_token("OPERATOR", yytext, yylineno); return ASSIGN; }
"=="        { add_token("OPERATOR", yytext, yylineno); return EQ; }
"!="        { add_token("OPERATOR", yytext, yylineno); return NEQ; }
"<"         { add_token("OPERATOR", yytext, yylineno); return LT; }
"<="        { add_token("OPERATOR", yytext, yylineno); return LTE; }
">"         { add_token("OPERATOR", yytext, yylineno); return GT; }
">="        { add_token("OPERATOR", yytext, yylineno); return GTE; }

"{"         { add_token("PUNCTUATION", yytext, yylineno); return LBRACE; }
"}"         { add_token("PUNCTUATION", yytext, yylineno); return RBRACE; }
"("         { add_token("PUNCTUATION", yytext, yylineno); return LPAREN; }
")"         { add_token("PUNCTUATION", yytext, yylineno); return RPAREN; }
";"         { add_token("PUNCTUATION", yytext, yylineno); return SEMI; }
","         { add_token("PUNCTUATION", yytext, yylineno); return COMMA; }

[ \t\r\n]+  { /* ignore whitespace */ }

.           { add_token("ERROR", yytext, yylineno); return yytext[0]; }
%%

void tokens_to_json() {
    printf("[\n");
    TokenList* curr = tokens_head;
    int first = 1;
    while(curr) {
        if(!first) printf(",\n");
        first = 0;
        printf("  {\"type\": \"%s\", \"value\": \"%s\", \"line\": %d}", curr->type, curr->value, curr->line);
        curr = curr->next;
    }
    printf("\n]");
}
