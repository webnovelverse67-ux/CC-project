#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "semantic.h"
#include "symtab.h"

extern int yylineno;

char* get_expr_type(ASTNode* node) {
    if (!node) return NULL;

    if (node->type == AST_LITERAL_INT) return "int";
    if (node->type == AST_LITERAL_FLOAT) return "float";

    if (node->type == AST_VAR) {
        Symbol* sym = symtab_lookup(node->name);
        if (!sym) {
            char buf[256];
            snprintf(buf, sizeof(buf), "Semantic Error: Undeclared variable '%s'", node->name);
            add_error(buf, yylineno); // Assuming yylineno is roughly correct, better to store line in AST
            return "unknown";
        }
        return sym->type;
    }

    if (node->type == AST_BINOP) {
        char* l_type = get_expr_type(node->left);
        char* r_type = get_expr_type(node->right);

        // Logical/Relational operators always return int (boolean)
        if (strcmp(node->name, "==") == 0 || strcmp(node->name, "!=") == 0 ||
            strcmp(node->name, "<") == 0 || strcmp(node->name, "<=") == 0 ||
            strcmp(node->name, ">") == 0 || strcmp(node->name, ">=") == 0) {
            return "int";
        }

        if (l_type && r_type) {
            if (strcmp(l_type, "float") == 0 || strcmp(r_type, "float") == 0) {
                return "float";
            }
            return "int";
        }
        return "unknown";
    }

    return "unknown";
}

void semantic_analysis(ASTNode* node) {
    if (!node) return;

    if (node->type == AST_BLOCK) {
        symtab_enter_scope();
        ASTNode* curr = node->left;
        while (curr) {
            semantic_analysis(curr);
            curr = curr->next;
        }
        symtab_exit_scope();
        return; // already traversed children
    }

    if (node->type == AST_VAR_DECL) {
        char* name = node->left->name;
        char* type = node->left->data_type;
        if (!symtab_insert(name, type)) {
            char buf[256];
            snprintf(buf, sizeof(buf), "Semantic Error: Redeclaration of variable '%s'", name);
            add_error(buf, yylineno);
        }
        if (node->right) {
            char* expr_type = get_expr_type(node->right);
            // Basic type checking warning (can be strict error if preferred)
            if (strcmp(type, "int") == 0 && strcmp(expr_type, "float") == 0) {
                char buf[256];
                snprintf(buf, sizeof(buf), "Semantic Warning: Implicit conversion from float to int for '%s'", name);
                add_error(buf, yylineno);
            }
        }
    }

    if (node->type == AST_ASSIGN) {
        char* name = node->left->name;
        Symbol* sym = symtab_lookup(name);
        if (!sym) {
            char buf[256];
            snprintf(buf, sizeof(buf), "Semantic Error: Undeclared variable '%s'", name);
            add_error(buf, yylineno);
        } else {
            char* expr_type = get_expr_type(node->right);
            if (strcmp(sym->type, "int") == 0 && strcmp(expr_type, "float") == 0) {
                char buf[256];
                snprintf(buf, sizeof(buf), "Semantic Warning: Implicit conversion from float to int for '%s'", name);
                add_error(buf, yylineno);
            }
        }
    }

    if (node->type == AST_IF || node->type == AST_WHILE) {
        get_expr_type(node->left); // check condition
    }

    if (node->type == AST_PRINT) {
        get_expr_type(node->left);
    }

    // Traverse children
    semantic_analysis(node->left);
    semantic_analysis(node->right);

    // In Program/Block, traverse next siblings if not block (handled above)
    if (node->type == AST_PROGRAM) {
        ASTNode* curr = node->left;
        while (curr) {
            semantic_analysis(curr);
            curr = curr->next;
        }
    }
}
