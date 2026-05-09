#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "codegen.h"

// Basic target code generation logic. Writes to out_file_path

FILE* out_file;
int indent_level = 0;

void print_indent() {
    for(int i=0; i<indent_level*4; i++) fprintf(out_file, " ");
}

void gen_cpp(ASTNode* node) {
    if(!node) return;

    if(node->type == AST_PROGRAM) {
        fprintf(out_file, "#include <iostream>\nusing namespace std;\n\nint main() {\n");
        indent_level++;
        ASTNode* curr = node->left;
        while(curr) {
            gen_cpp(curr);
            curr = curr->next;
        }
        indent_level--;
        fprintf(out_file, "    return 0;\n}\n");
    }
    else if(node->type == AST_BLOCK) {
        fprintf(out_file, "{\n");
        indent_level++;
        ASTNode* curr = node->left;
        while(curr) {
            gen_cpp(curr);
            curr = curr->next;
        }
        indent_level--;
        print_indent();
        fprintf(out_file, "}\n");
    }
    else if(node->type == AST_VAR_DECL) {
        print_indent();
        fprintf(out_file, "%s %s", node->left->data_type, node->left->name);
        if(node->right) {
            fprintf(out_file, " = ");
            gen_cpp(node->right);
        }
        fprintf(out_file, ";\n");
    }
    else if(node->type == AST_ASSIGN) {
        print_indent();
        fprintf(out_file, "%s = ", node->left->name);
        gen_cpp(node->right);
        fprintf(out_file, ";\n");
    }
    else if(node->type == AST_PRINT) {
        print_indent();
        fprintf(out_file, "cout << ");
        gen_cpp(node->left);
        fprintf(out_file, " << endl;\n");
    }
    else if(node->type == AST_IF) {
        print_indent();
        fprintf(out_file, "if (");
        gen_cpp(node->left);
        fprintf(out_file, ") ");
        if(node->right->type != AST_BLOCK) fprintf(out_file, "\n");
        gen_cpp(node->right);

        if(node->right->next) { // else part
            print_indent();
            fprintf(out_file, "else ");
            if(node->right->next->type != AST_BLOCK) fprintf(out_file, "\n");
            gen_cpp(node->right->next);
        }
    }
    else if(node->type == AST_WHILE) {
        print_indent();
        fprintf(out_file, "while (");
        gen_cpp(node->left);
        fprintf(out_file, ") ");
        if(node->right->type != AST_BLOCK) fprintf(out_file, "\n");
        gen_cpp(node->right);
    }
    else if(node->type == AST_BINOP) {
        gen_cpp(node->left);
        fprintf(out_file, " %s ", node->name);
        gen_cpp(node->right);
    }
    else if(node->type == AST_VAR) {
        fprintf(out_file, "%s", node->name);
    }
    else if(node->type == AST_LITERAL_INT) {
        fprintf(out_file, "%d", node->int_val);
    }
    else if(node->type == AST_LITERAL_FLOAT) {
        fprintf(out_file, "%f", node->float_val);
    }
}

void gen_python(ASTNode* node) {
    if(!node) return;

    if(node->type == AST_PROGRAM) {
        ASTNode* curr = node->left;
        while(curr) {
            gen_python(curr);
            curr = curr->next;
        }
    }
    else if(node->type == AST_BLOCK) {
        indent_level++;
        ASTNode* curr = node->left;
        if(!curr) {
            print_indent();
            fprintf(out_file, "pass\n");
        }
        while(curr) {
            gen_python(curr);
            curr = curr->next;
        }
        indent_level--;
    }
    else if(node->type == AST_VAR_DECL) {
        print_indent();
        if(node->right) {
            fprintf(out_file, "%s = ", node->left->name);
            gen_python(node->right);
            fprintf(out_file, "\n");
        } else {
            fprintf(out_file, "%s = None\n", node->left->name);
        }
    }
    else if(node->type == AST_ASSIGN) {
        print_indent();
        fprintf(out_file, "%s = ", node->left->name);
        gen_python(node->right);
        fprintf(out_file, "\n");
    }
    else if(node->type == AST_PRINT) {
        print_indent();
        fprintf(out_file, "print(");
        gen_python(node->left);
        fprintf(out_file, ")\n");
    }
    else if(node->type == AST_IF) {
        print_indent();
        fprintf(out_file, "if ");
        gen_python(node->left);
        fprintf(out_file, ":\n");
        if(node->right->type == AST_BLOCK) {
            gen_python(node->right);
        } else {
            indent_level++;
            gen_python(node->right);
            indent_level--;
        }

        if(node->right->next) { // else part
            print_indent();
            fprintf(out_file, "else:\n");
            if(node->right->next->type == AST_BLOCK) {
                gen_python(node->right->next);
            } else {
                indent_level++;
                gen_python(node->right->next);
                indent_level--;
            }
        }
    }
    else if(node->type == AST_WHILE) {
        print_indent();
        fprintf(out_file, "while ");
        gen_python(node->left);
        fprintf(out_file, ":\n");
        if(node->right->type == AST_BLOCK) {
            gen_python(node->right);
        } else {
            indent_level++;
            gen_python(node->right);
            indent_level--;
        }
    }
    else if(node->type == AST_BINOP) {
        gen_python(node->left);
        fprintf(out_file, " %s ", node->name);
        gen_python(node->right);
    }
    else if(node->type == AST_VAR) {
        fprintf(out_file, "%s", node->name);
    }
    else if(node->type == AST_LITERAL_INT) {
        fprintf(out_file, "%d", node->int_val);
    }
    else if(node->type == AST_LITERAL_FLOAT) {
        fprintf(out_file, "%f", node->float_val);
    }
}

void generate_target_code(ASTNode* node, const char* lang, const char* out_file_path) {
    out_file = fopen(out_file_path, "w");
    if(!out_file) return;

    if(strcmp(lang, "python") == 0 || strcmp(lang, "Python") == 0) {
        gen_python(node);
    } else {
        gen_cpp(node);
    }

    fclose(out_file);
}
