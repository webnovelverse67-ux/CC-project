#ifndef AST_H
#define AST_H

typedef enum {
    AST_PROGRAM,
    AST_BLOCK,
    AST_VAR_DECL,
    AST_ASSIGN,
    AST_IF,
    AST_WHILE,
    AST_BINOP,
    AST_UNOP,
    AST_VAR,
    AST_LITERAL_INT,
    AST_LITERAL_FLOAT,
    AST_PRINT
} ASTNodeType;

typedef struct ASTNode {
    ASTNodeType type;
    char* name;
    int int_val;
    float float_val;
    char* data_type;
    struct ASTNode* left;
    struct ASTNode* right;
    struct ASTNode* next;
} ASTNode;

ASTNode* create_node(ASTNodeType type, ASTNode* left, ASTNode* right);
ASTNode* create_var(char* name);
ASTNode* create_literal_int(int val);
ASTNode* create_literal_float(float val);
ASTNode* create_binop(char* op, ASTNode* left, ASTNode* right);
ASTNode* append_sibling(ASTNode* node, ASTNode* sibling);

void ast_to_json(ASTNode* node, int indent);

#endif
