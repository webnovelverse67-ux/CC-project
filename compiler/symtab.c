#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "symtab.h"

#define HASH_SIZE 100

Symbol* hash_table[HASH_SIZE];
int current_scope = 0;

int hash(char* name) {
    int sum = 0;
    while (*name) sum += *name++;
    return sum % HASH_SIZE;
}

void symtab_enter_scope() {
    current_scope++;
}

void symtab_exit_scope() {
    for (int i = 0; i < HASH_SIZE; i++) {
        Symbol* curr = hash_table[i];
        Symbol* prev = NULL;
        while (curr) {
            if (curr->scope_level == current_scope) {
                if (prev) prev->next = curr->next;
                else hash_table[i] = curr->next;
                Symbol* temp = curr;
                curr = curr->next;
                free(temp->name);
                free(temp->type);
                free(temp);
            } else {
                prev = curr;
                curr = curr->next;
            }
        }
    }
    current_scope--;
}

int symtab_insert(char* name, char* type) {
    int index = hash(name);
    Symbol* curr = hash_table[index];
    while (curr) {
        if (strcmp(curr->name, name) == 0 && curr->scope_level == current_scope) {
            return 0; // Redeclaration error
        }
        curr = curr->next;
    }

    Symbol* new_sym = (Symbol*)malloc(sizeof(Symbol));
    new_sym->name = strdup(name);
    new_sym->type = strdup(type);
    new_sym->scope_level = current_scope;
    new_sym->next = hash_table[index];
    hash_table[index] = new_sym;
    return 1;
}

Symbol* symtab_lookup(char* name) {
    int index = hash(name);
    Symbol* curr = hash_table[index];
    Symbol* best_match = NULL;

    while (curr) {
        if (strcmp(curr->name, name) == 0) {
            if (!best_match || curr->scope_level > best_match->scope_level) {
                best_match = curr;
            }
        }
        curr = curr->next;
    }
    return best_match;
}

void symtab_to_json() {
    printf("[\n");
    int first = 1;
    for (int i = 0; i < HASH_SIZE; i++) {
        Symbol* curr = hash_table[i];
        while (curr) {
            if (!first) printf(",\n");
            first = 0;
            printf("  {\"name\": \"%s\", \"type\": \"%s\", \"scope\": %d}", curr->name, curr->type, curr->scope_level);
            curr = curr->next;
        }
    }
    printf("\n]");
}
