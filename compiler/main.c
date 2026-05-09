#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"
#include "symtab.h"
#include "tac.h"
#include "semantic.h"
#include "codegen.h"

extern FILE* yyin;
extern int yyparse();
extern ASTNode* root;

extern void tokens_to_json();
extern void errors_to_json();

int main(int argc, char** argv) {
    if (argc < 3) {
        printf("Usage: %s <target_language> <out_file>\n", argv[0]);
        return 1;
    }

    char* target_lang = argv[1];
    char* out_file = argv[2];

    // Read from stdin if no file provided (for backend piping)
    // Actually we'll pipe to stdin from backend
    // yyin = stdin;

    yyparse();

    // Perform Semantic Analysis
    semantic_analysis(root);

    // Generate TAC
    generate_tac(root);

    // Generate Target Code to out_file
    generate_target_code(root, target_lang, out_file);

    // Output JSON
    printf("{\n");

    printf("\"tokens\": ");
    tokens_to_json();
    printf(",\n");

    printf("\"ast\": ");
    ast_to_json(root, 0);
    printf(",\n");

    printf("\"symbol_table\": ");
    symtab_to_json();
    printf(",\n");

    printf("\"errors\": ");
    errors_to_json();
    printf(",\n");

    printf("\"tac\": ");
    tac_to_json();
    printf(",\n");

    // Read target code and embed in json
    printf("\"target_code\": \"");
    FILE* f = fopen(out_file, "r");
    if (f) {
        char ch;
        while ((ch = fgetc(f)) != EOF) {
            if (ch == '\n') printf("\\n");
            else if (ch == '"') printf("\\\"");
            else if (ch == '\\') printf("\\\\");
            else printf("%c", ch);
        }
        fclose(f);
    }
    printf("\"\n");

    printf("}\n");

    return 0;
}
