#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "../../include/parser.h"
#include "../../include/semantic.h"

// =============== BEGIN STEP 4 ===============
void semantic_error(SemanticErrorType error, const char *name, int line) {
    printf("Semantic Error at line %d: ", line);

    switch (error) {
        case SEM_ERROR_UNDECLARED_VARIABLE:
            printf("Undeclared variable '%s'\n", name);
            break;
        case SEM_ERROR_REDECLARED_VARIABLE:
            printf("Variable '%s' already declared in this scope\n", name);
            break;
        case SEM_ERROR_TYPE_MISMATCH:
            printf("Type mismatch involving '%s'\n", name);
            break;
        case SEM_ERROR_UNINITIALIZED_VARIABLE:
            printf("Variable '%s' may be used uninitialized\n", name);
            break;
        case SEM_ERROR_INVALID_OPERATION:
            printf("Invalid operation involving '%s'\n", name);
            break;
        default:
            printf("Unknown semantic error with '%s'\n", name);
    }
}

// =============== END STEP 4 ===============

// =============== BEGIN STEP 2 ===============
// Initialize a new symbol table
// Creates an empty symbol table structure with scope level set to 0
SymbolTable *init_symbol_table() {
    SymbolTable *table = malloc(sizeof(SymbolTable));
    if (table) {
        table->head = NULL;
        table->current_scope = 0;
    }
    return table;
}

void symbol_table_dump(SymbolTable *table) {
    printf("== SYMBOL TABLE DUMP ==\n");
    printf("Total symbols: %lu\n\n", sizeof(*table) / sizeof(table[0]));
    unsigned int index = 0;
    Symbol *current_symbol = table->head;
    while (current_symbol != NULL) {
        printf("\tSymbol[%u]\n", index);
        printf("\tName: %s\n", current_symbol->name);
        printf("\tType: %d\n", current_symbol->type);
        printf("\tLine Declared: %d\n", current_symbol->line_declared);
        if (current_symbol->is_initialized) {
            printf("\tInitialized: Yes\n");
        } else {
            printf("\tInitialized: No\n");
        }
        index++;
        current_symbol = current_symbol->next;
    }
    printf("===================\n");
}

// Add a symbol to the table
// Inserts a new variable with given name, type, and line number into the current scope
void add_symbol(SymbolTable *table, const char *name, int type, int line) {
    Symbol *symbol = malloc(sizeof(Symbol));
    if (symbol) {
        strcpy(symbol->name, name);
        symbol->type = type;
        symbol->scope_level = table->current_scope;
        symbol->line_declared = line;
        symbol->is_initialized = 0;

        // Add to beginning of list
        symbol->next = table->head;
        table->head = symbol;
    }
}

// Look up a symbol in the table
// Searches for a variable by name across all accessible scopes
// Returns the symbol if found, NULL otherwise
// Symbol *lookup_symbol(SymbolTable *table, const char *name) {
//     Symbol *current = table->head;
//     while (current) {
//         if (strcmp(current->name, name) == 0) {
//             return current;
//         }
//         current = current->next;
//     }
//     return NULL;
// }

Symbol *lookup_symbol(SymbolTable *table, const char *name) {
    Symbol *current = table->head;
    Symbol *best_match = NULL;

    while (current) {
        if (strcmp(current->name, name) == 0) {
            if (current->scope_level <= table->current_scope) {
                best_match = current;
            }
        }
        current = current->next;
    }
    return best_match;
}

// Look up symbol in current scope only
Symbol *lookup_symbol_current_scope(SymbolTable *table, const char *name) {
    Symbol *current = table->head;
    while (current) {
        if (strcmp(current->name, name) == 0 &&
            current->scope_level == table->current_scope) {
            return current;
        }
        current = current->next;
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
// void exit_scope(SymbolTable *table) {
//     table->current_scope--;
// }

void exit_scope(SymbolTable *table) {
    Symbol **previous = &table->head;
    Symbol *current = table->head;

    while (current != NULL) {
        if (current->scope_level == table->current_scope) {
            *previous = current->next;
            free(current);
            current = *previous;
        } else {
            previous = &current->next;
            current = current->next;
        }
    }
    table->current_scope--;
}

// Remove symbols from the current scope
// Cleans up symbols that are no longer accessible after leaving a scope
void remove_symbols_in_current_scope(SymbolTable *table) {
    Symbol *current_symbol = table->head;
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

// =============== END STEP 2 ===============

// =============== BEGIN STEP 3 ===============

// Analyze AST semantically
int analyze_semantics(ASTNode *ast) {
    SymbolTable *table = init_symbol_table();
    int result = check_program(ast, table);
    if (result)
        symbol_table_dump(table);
    free_symbol_table(table);
    return result;
}

// Check program node
int check_program(ASTNode *node, SymbolTable *table) {
    if (!node) return 1;

    int result = 1;

    if (node->type == AST_PROGRAM) {
        // Check left child (statement)
        if (node->left) {
            result = check_statement(node->left, table) && result;
        }

        // Check right child (rest of program)
        if (node->right) {
            result = check_program(node->right, table) && result;
        }
    }
    return result;
}

// Check declaration node
int check_declaration(ASTNode *node, SymbolTable *table) {
    if (node->type != AST_VARDECL) {
        return 0;
    }

    const char *name = node->token.lexeme;

    // Check if variable already declared in current scope
    Symbol *existing = lookup_symbol_current_scope(table, name);
    if (existing) {
        semantic_error(SEM_ERROR_REDECLARED_VARIABLE, name, node->token.line);
        return 0;
    }

    // Add to symbol table
    add_symbol(table, name, TOKEN_INT, node->token.line);
    return 1;
}

// int check_statement(ASTNode *node, SymbolTable *table) {
//     check_declaration(node, table);
//     check_assignment(node, table);
//     return 1;
// }

int check_statement(ASTNode *node, SymbolTable *table) {
    if (!node) return 1;

    if (node->type == AST_IF || node->type == AST_WHILE) {
        return check_expression(node->left, table) && check_statement(node->right, table);
    } else if (node->type == AST_BLOCK) {
        enter_scope(table);
        int result = check_statement(node->left, table) && check_statement(node->right, table);
        exit_scope(table);
        return result;
    } else if (node->type == AST_VARDECL) {
        return check_declaration(node, table);
    } else if (node->type == AST_ASSIGN) {
        return check_assignment(node, table);
    } else if (node->type == AST_PRINT) {
        const char *name = node->left->token.lexeme;
        Symbol *symbol = lookup_symbol(table, name);
        if (!symbol) {
            semantic_error(SEM_ERROR_UNDECLARED_VARIABLE, name, node->token.line);
            return 0;
        }
    }
    return 1;
}

int check_type_compatability(ASTNode *node, SymbolTable *table) {
    ASTNode *left = node->left;
    ASTNode *right = node->right;
    if (!left || !right) {
        return 0;
    }

    switch (node->type) {
        case AST_COMPARISONOP:
        case AST_BINOP:
        case AST_BOOLOP: {
            if (left->type == AST_NUMBER && right->type == AST_NUMBER) {
                return 1;
            }

            Symbol *left_symbol;
            Symbol *right_symbol;

            if (left->type == AST_IDENTIFIER) {
                left_symbol = lookup_symbol(table, left->token.lexeme);
                if (right->type == AST_NUMBER &&
                    (left_symbol->type == TOKEN_INT || left_symbol->type == TOKEN_FLOAT || left_symbol->type ==
                     TOKEN_CHAR || left_symbol->type == TOKEN_DOUBLE)) {
                    return 1;
                }
            }

            if (right->type == AST_IDENTIFIER) {
                right_symbol = lookup_symbol(table, right->token.lexeme);
                if (left->type == AST_NUMBER &&
                    (right_symbol->type == TOKEN_INT || right_symbol->type == TOKEN_FLOAT || right_symbol->type ==
                     TOKEN_CHAR || right_symbol->type == TOKEN_DOUBLE)) {
                    return 1;
                }
            }
            return 0;
        }
        default:
            return 0;
    }
}

int check_expression(ASTNode *node, SymbolTable *table) {
    switch (node->type) {
        case AST_NUMBER:
            return 1;
        case AST_IDENTIFIER: {
            const char *name = node->token.lexeme;
            // Lookup the symbol of the current variable in the statement
            Symbol *existing = lookup_symbol(table, name);
            // Check if it exists
            if (!existing) {
                semantic_error(SEM_ERROR_UNDECLARED_VARIABLE, name, node->token.line);
                return 0;
            } else {
                if (!existing->is_initialized) {
                    semantic_error(SEM_ERROR_UNINITIALIZED_VARIABLE, name, node->token.line);
                    return 0;
                }
            }
            return 1;
        }
        case AST_FACTORIAL:
            break;
        case AST_ADDRESS_OF:
            break;
        case AST_BINOP:
        case AST_COMPARISONOP:
        case AST_BOOLOP: {
            if (!check_type_compatability(node, table)) {
                semantic_error(SEM_ERROR_TYPE_MISMATCH, node->token.lexeme, node->token.line);
            }
            return check_expression(node->left, table) && check_expression(node->right, table);
        }
        case AST_FUNCDECL:
            break;
        default:
            return 0;
    }
}

// Check assignment node
int check_assignment(ASTNode *node, SymbolTable *table) {
    if (node->type != AST_ASSIGN || !node->left || !node->right) {
        return 0;
    }

    const char *name = node->left->token.lexeme;

    // Check if variable exists
    Symbol *symbol = lookup_symbol(table, name);
    if (!symbol) {
        semantic_error(SEM_ERROR_UNDECLARED_VARIABLE, name, node->token.line);
        return 0;
    }

    // Check expression
    int expr_valid = check_expression(node->right, table);

    // Mark as initialized
    if (expr_valid) {
        symbol->is_initialized = 1;
    }

    return expr_valid;
}

// =============== END STEP 3 ===============

int main() {
    const char *input =
            "x = 42;\n"
            "if (x > y) {\n"
            "    int y;\n"
            "    y = z + 10;\n"
            "    print y;\n"
            "}\n";

    // const char *input =
    //         "// Uninitialized variable\n"
    //         "int x;\n"
    //         "int y;\n"
    //         "y = x + 5; // Warning: x used before initialization\n";
    //"print y;";

    // const char *input = "int x;\n"
    //         "x = 42;\n"
    //         "y = 45;\n";

    printf("Analyzing input:\n%s\n", input);

    // Lexical analysis and parsing
    parser_init(input);
    ASTNode *ast = parse();

    printf("\nAbstract Syntax Tree:\n");
    print_ast(ast, 0);

    printf("AST created. Performing semantic analysis...\n\n");

    // Semantic analysis
    int result = analyze_semantics(ast);

    if (result) {
        printf("Semantic analysis successful. No errors found.\n");
    } else {
        printf("Semantic analysis failed. Errors detected.\n");
    }

    // Clean up
    free_ast(ast);

    return 0;
}
