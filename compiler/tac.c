#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tac.h"

TAC* tac_head = NULL;
TAC* tac_tail = NULL;
int temp_count = 0;
int label_count = 0;

TAC* create_tac(char* op, char* arg1, char* arg2, char* res) {
    TAC* tac = (TAC*)malloc(sizeof(TAC));
    tac->op = op ? strdup(op) : NULL;
    tac->arg1 = arg1 ? strdup(arg1) : NULL;
    tac->arg2 = arg2 ? strdup(arg2) : NULL;
    tac->res = res ? strdup(res) : NULL;
    tac->next = NULL;
    return tac;
}

void append_tac(TAC* tac) {
    if (!tac_head) {
        tac_head = tac;
        tac_tail = tac;
    } else {
        tac_tail->next = tac;
        tac_tail = tac;
    }
}

char* new_temp() {
    char buf[16];
    sprintf(buf, "t%d", temp_count++);
    return strdup(buf);
}

char* new_label() {
    char buf[16];
    sprintf(buf, "L%d", label_count++);
    return strdup(buf);
}

char* generate_tac_helper(ASTNode* node) {
    if (!node) return NULL;

    if (node->type == AST_LITERAL_INT) {
        char buf[32];
        sprintf(buf, "%d", node->int_val);
        return strdup(buf);
    }

    if (node->type == AST_LITERAL_FLOAT) {
        char buf[32];
        sprintf(buf, "%f", node->float_val);
        return strdup(buf);
    }

    if (node->type == AST_VAR) {
        return strdup(node->name);
    }

    if (node->type == AST_BINOP) {
        char* l = generate_tac_helper(node->left);
        char* r = generate_tac_helper(node->right);
        char* t = new_temp();
        append_tac(create_tac(node->name, l, r, t));
        return t;
    }

    if (node->type == AST_ASSIGN) {
        char* val = generate_tac_helper(node->right);
        append_tac(create_tac("=", val, NULL, node->left->name));
        return NULL;
    }

    if (node->type == AST_VAR_DECL) {
        if (node->right) {
            char* val = generate_tac_helper(node->right);
            append_tac(create_tac("=", val, NULL, node->left->name));
        }
        return NULL;
    }

    if (node->type == AST_PRINT) {
        char* val = generate_tac_helper(node->left);
        append_tac(create_tac("print", val, NULL, NULL));
        return NULL;
    }

    if (node->type == AST_IF) {
        char* cond = generate_tac_helper(node->left);
        char* l_true = new_label();
        char* l_end = new_label();

        append_tac(create_tac("if_goto", cond, NULL, l_true));

        if (node->right && node->right->next) {
            // has else (can be block or single statement)
            char* l_else = new_label();
            append_tac(create_tac("goto", NULL, NULL, l_else));

            append_tac(create_tac("label", NULL, NULL, l_true));
            generate_tac_helper(node->right);
            append_tac(create_tac("goto", NULL, NULL, l_end));

            append_tac(create_tac("label", NULL, NULL, l_else));
            generate_tac_helper(node->right->next);
        } else {
            append_tac(create_tac("goto", NULL, NULL, l_end));
            append_tac(create_tac("label", NULL, NULL, l_true));
            generate_tac_helper(node->right);
        }

        append_tac(create_tac("label", NULL, NULL, l_end));
        return NULL;
    }

    if (node->type == AST_WHILE) {
        char* l_start = new_label();
        char* l_true = new_label();
        char* l_end = new_label();

        append_tac(create_tac("label", NULL, NULL, l_start));
        char* cond = generate_tac_helper(node->left);
        append_tac(create_tac("if_goto", cond, NULL, l_true));
        append_tac(create_tac("goto", NULL, NULL, l_end));

        append_tac(create_tac("label", NULL, NULL, l_true));
        generate_tac_helper(node->right);
        append_tac(create_tac("goto", NULL, NULL, l_start));

        append_tac(create_tac("label", NULL, NULL, l_end));
        return NULL;
    }

    if (node->type == AST_PROGRAM || node->type == AST_BLOCK) {
        ASTNode* curr = node->left;
        while(curr) {
            generate_tac_helper(curr);
            curr = curr->next;
        }
    }
    return NULL;
}

void generate_tac(ASTNode* node) {
    generate_tac_helper(node);
}

void tac_to_json() {
    printf("[\n");
    TAC* curr = tac_head;
    int first = 1;
    while(curr) {
        if(!first) printf(",\n");
        first = 0;
        printf("  {\"op\": \"%s\"", curr->op ? curr->op : "");
        if(curr->arg1) printf(", \"arg1\": \"%s\"", curr->arg1);
        else printf(", \"arg1\": null");
        if(curr->arg2) printf(", \"arg2\": \"%s\"", curr->arg2);
        else printf(", \"arg2\": null");
        if(curr->res) printf(", \"res\": \"%s\"", curr->res);
        else printf(", \"res\": null");
        printf("}");
        curr = curr->next;
    }
    printf("\n]");
}
