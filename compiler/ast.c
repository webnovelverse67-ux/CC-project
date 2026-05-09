#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"

ASTNode* create_node(ASTNodeType type, ASTNode* left, ASTNode* right) {
    ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
    node->type = type;
    node->name = NULL;
    node->data_type = NULL;
    node->left = left;
    node->right = right;
    node->next = NULL;
    return node;
}

ASTNode* create_var(char* name) {
    ASTNode* node = create_node(AST_VAR, NULL, NULL);
    node->name = strdup(name);
    return node;
}

ASTNode* create_literal_int(int val) {
    ASTNode* node = create_node(AST_LITERAL_INT, NULL, NULL);
    node->int_val = val;
    return node;
}

ASTNode* create_literal_float(float val) {
    ASTNode* node = create_node(AST_LITERAL_FLOAT, NULL, NULL);
    node->float_val = val;
    return node;
}

ASTNode* create_binop(char* op, ASTNode* left, ASTNode* right) {
    ASTNode* node = create_node(AST_BINOP, left, right);
    node->name = strdup(op);
    return node;
}

ASTNode* append_sibling(ASTNode* node, ASTNode* sibling) {
    if (!node) return sibling;
    ASTNode* cur = node;
    while (cur->next) {
        cur = cur->next;
    }
    cur->next = sibling;
    return node;
}

void ast_to_json(ASTNode* node, int indent) {
    if (!node) {
        printf("null");
        return;
    }
    printf("{\n");
    for (int i = 0; i < indent + 2; i++) printf(" ");
    printf("\"type\": ");
    switch (node->type) {
        case AST_PROGRAM: printf("\"Program\""); break;
        case AST_BLOCK: printf("\"Block\""); break;
        case AST_VAR_DECL: printf("\"VarDecl\""); break;
        case AST_ASSIGN: printf("\"Assign\""); break;
        case AST_IF: printf("\"If\""); break;
        case AST_WHILE: printf("\"While\""); break;
        case AST_BINOP: printf("\"BinOp\""); break;
        case AST_VAR: printf("\"Var\""); break;
        case AST_LITERAL_INT: printf("\"LiteralInt\""); break;
        case AST_LITERAL_FLOAT: printf("\"LiteralFloat\""); break;
        case AST_PRINT: printf("\"Print\""); break;
        default: printf("\"Unknown\""); break;
    }

    if (node->name) {
        printf(",\n");
        for (int i = 0; i < indent + 2; i++) printf(" ");
        printf("\"name\": \"%s\"", node->name);
    }
    if (node->type == AST_LITERAL_INT) {
        printf(",\n");
        for (int i = 0; i < indent + 2; i++) printf(" ");
        printf("\"value\": %d", node->int_val);
    }
    if (node->type == AST_LITERAL_FLOAT) {
        printf(",\n");
        for (int i = 0; i < indent + 2; i++) printf(" ");
        printf("\"value\": %f", node->float_val);
    }

    if (node->left) {
        printf(",\n");
        for (int i = 0; i < indent + 2; i++) printf(" ");
        printf("\"left\": ");
        ast_to_json(node->left, indent + 2);
    }
    if (node->right) {
        printf(",\n");
        for (int i = 0; i < indent + 2; i++) printf(" ");
        printf("\"right\": ");
        ast_to_json(node->right, indent + 2);
    }
    if (node->next) {
        printf(",\n");
        for (int i = 0; i < indent + 2; i++) printf(" ");
        printf("\"next\": ");
        ast_to_json(node->next, indent + 2);
    }

    printf("\n");
    for (int i = 0; i < indent; i++) printf(" ");
    printf("}");
}
