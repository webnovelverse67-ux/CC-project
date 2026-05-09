#ifndef SYMTAB_H
#define SYMTAB_H

typedef struct Symbol {
    char* name;
    char* type;
    int scope_level;
    struct Symbol* next;
} Symbol;

void symtab_enter_scope();
void symtab_exit_scope();
int symtab_insert(char* name, char* type);
Symbol* symtab_lookup(char* name);

void symtab_to_json();

#endif
