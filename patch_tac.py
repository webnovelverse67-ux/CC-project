with open("compiler/tac.c", "r") as f:
    content = f.read()

content = content.replace(
'''        if (node->right && node->right->next && node->right->next->type == AST_BLOCK) {
            // has else
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
        }''',
'''        if (node->right && node->right->next) {
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
        }'''
)

with open("compiler/tac.c", "w") as f:
    f.write(content)
