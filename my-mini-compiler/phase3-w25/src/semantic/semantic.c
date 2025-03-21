#include <stdlib.h>
#include <string.h>
#include "semantic.h"

// Initialize a new symbol table
// Creates an empty symbol table structure with scope level set to 0
SymbolTable *init_symbol_table() {
    SymbolTable *symbol_table = malloc(sizeof(SymbolTable));
    symbol_table->head = NULL;
    symbol_table->current_scope = 0;
    return symbol_table;
}

// Add a symbol to the table
// Inserts a new variable with given name, type, and line number into the current scope
void add_symbol(SymbolTable *table, const char *name, int type, int line) {
    Symbol *new_symbol = malloc(sizeof(Symbol));

    strcpy(new_symbol->name, name);
    new_symbol->type = type;
    new_symbol->line_declared = line;


    if (table->head == NULL) {
        table->head = new_symbol;
    } else {
        Symbol *current_symbol = table->head;
        while (table->head->next != NULL) {
            current_symbol = current_symbol->next;
        }

        current_symbol->next = new_symbol;
    }
}

// Look up a symbol in the table
// Searches for a variable by name across all accessible scopes
// Returns the symbol if found, NULL otherwise
Symbol *lookup_symbol(SymbolTable *table, const char *name) {
    Symbol *current_symbol = table->head;
    while (current_symbol != NULL) {
        if (strcmp(current_symbol->name, name) == 0) {
            return current_symbol;
        }
    }

    return NULL;
}

// Enter a new scope level
// Increments the current scope level when entering a block (e.g., if, while)
void enter_scope(SymbolTable *table) {
    table->current_scope++;
}

// Exit the current scope
// Decrements the current scope level when leaving a block
// Optionally removes symbols that are no longer in scope
void exit_scope(SymbolTable *table) {
    table->current_scope--;
}

// Remove symbols from the current scope
// Cleans up symbols that are no longer accessible after leaving a scope
void remove_symbols_in_current_scope(SymbolTable *table) {
    Symbol* current_symbol = table->head;
    while (current_symbol != NULL) {
        if (current_symbol->scope_level == table->current_scope) {
            free(current_symbol);
        }
        current_symbol = current_symbol->next;
    }
}

// Free the symbol table memory
// Releases all allocated memory when the symbol table is no longer needed
void free_symbol_table(SymbolTable *table) {
    Symbol *current_symbol = table->head;
    while (current_symbol != NULL) {
        free(current_symbol);
        current_symbol = current_symbol->next;
    }
}

int main() {
    return 0;
}
