/* parser.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../../include/lexer.h"
#include "../../include/parser.h"


// TODO 1: Add more parsing function declarations for:
// - if statements: if (condition) { ... }
// - while loops: while (condition) { ... }
// - repeat-until: repeat { ... } until (condition)
// - print statements: print x;
// - blocks: { statement1; statement2; }
// - factorial function: factorial(x)
//------------------------------------------------------------------------------------------------------------------------Added code under
static ASTNode *parse_if_statement(void);

static ASTNode *parse_while_statement(void);

static ASTNode *parse_repeat_statement(void);

static ASTNode *parse_print_statement(void);

static ASTNode *parse_block(void);

static ASTNode *parse_factorial(void);

static ASTNode *parse_functions(void);

//------------------------------------------------------------------------------------------------------------------------Added code above

// Current token being processed
static Token current_token;
static int position = 0;
static const char *source;

static void parse_error(ParseError error, Token token) {
    // TODO 2: Add more error types for:
    // - Missing parentheses
    // - Missing condition
    // - Missing block braces
    // - Invalid operator
    // - Function call errors

    printf("Parse Error at line %d: ", token.line);
    switch (error) {
        case PARSE_ERROR_UNEXPECTED_TOKEN:
            printf("Unexpected token '%s'\n", token.lexeme);
            break;
        case PARSE_ERROR_MISSING_SEMICOLON:
            printf("Missing semicolon after '%s'\n", token.lexeme);
            break;
        case PARSE_ERROR_MISSING_IDENTIFIER:
            printf("Expected identifier after '%s'\n", token.lexeme);
            break;
        case PARSE_ERROR_MISSING_EQUALS:
            printf("Expected '=' after '%s'\n", token.lexeme);
            break;
        case PARSE_ERROR_INVALID_EXPRESSION:
            printf("Invalid expression after '%s'\n", token.lexeme);
            break;
        //------------------------------------------------------------------------------------------------------------------------Added code under
        case PARSE_ERROR_MISSING_PARENTHESIS:
            printf("Missing parenthesis after '%s'\n", token.lexeme);
            break;
        case PARSE_ERROR_MISSING_CONDITION:
            printf("Missing condition after '%s'\n", token.lexeme);
            break;
        case PARSE_ERROR_MISSING_BLOCK_BRACES:
            printf("Missing block braces after '%s'\n", token.lexeme);
            break;
        case PARSE_ERROR_INVALID_OPERATOR:
            printf("Invalid operator: '%s'\n", token.lexeme);
            break;
        case PARSE_ERROR_FUNCTION_CALL_ERROR:
            printf("Invalid function call '%s'\n", token.lexeme);
            break;
        case PARSE_ERROR_UNDECLARED_VARIABLE:
            printf("Variable '%s' is not declared in scope\n", token.lexeme);
            break;
        //------------------------------------------------------------------------------------------------------------------------Added code above
        default:
            printf("Unknown error\n");
    }
}

// Get next token
static void advance(void) {
    current_token = get_next_token(source, &position);
}

// Create a new AST node
static ASTNode *create_node(ASTNodeType type) {
    ASTNode *node = malloc(sizeof(ASTNode));
    if (node) {
        node->type = type;
        node->token = current_token;
        node->left = NULL;
        node->right = NULL;
    }
    return node;
}

// Match current token with expected type
static int match(TokenType type) {
    return current_token.type == type;
}

// Function to handle errors such that it skips ahead so it can resume parsing to allow for more errors to be collected at once.
static void error_recovery() {
    // Skip tokens until a safe point is found.
    while (current_token.type != TOKEN_EOF) {
        if (match(TOKEN_SEMICOLON)) {
            advance();
            break;
        }
        // Check if current token is the beginning of a new statement.
        if (current_token.type == TOKEN_INT ||
            current_token.type == TOKEN_CHAR ||
            current_token.type == TOKEN_FLOAT ||
            current_token.type == TOKEN_DOUBLE ||
            current_token.type == TOKEN_STRING ||
            current_token.type == TOKEN_IF ||
            current_token.type == TOKEN_WHILE ||
            current_token.type == TOKEN_REPEAT ||
            current_token.type == TOKEN_PRINT ||
            current_token.type == TOKEN_IDENTIFIER ||
            current_token.type == TOKEN_LBRACE) {
            break;
        }
        advance();
    }
}

// Expect a token type or error
static void expect(TokenType type) {
    if (match(type)) {
        advance();
    } else {
        parse_error(PARSE_ERROR_UNEXPECTED_TOKEN, current_token);
        error_recovery();
    }
}

// Forward declarations
// static ASTNode *parse_statement(void);
// static ASTNode *parse_expression(void);

// TODO 3: Add parsing functions for each new statement type
// static ASTNode* parse_if_statement(void) { ... }
// static ASTNode* parse_while_statement(void) { ... }
// static ASTNode* parse_repeat_statement(void) { ... }
// static ASTNode* parse_print_statement(void) { ... }
// static ASTNode* parse_block(void) { ... }
// static ASTNode* parse_factorial(void) { ... }

//------------------------------------------------------------------------------------------------------------------------Added code under
static ASTNode *parse_if_statement(void) {
    advance(); // consume 'if'
    expect(TOKEN_LPAREN);
    ASTNode *condition = parse_bool();
    expect(TOKEN_RPAREN);
    ASTNode *body = parse_block();

    ASTNode *node = create_node(AST_IF);
    node->left = condition;
    node->right = body;
    return node;
}

static ASTNode *parse_while_statement(void) {
    advance(); // consume 'while'
    expect(TOKEN_LPAREN);
    ASTNode *condition = parse_bool();
    expect(TOKEN_RPAREN);
    ASTNode *body = parse_block();

    ASTNode *node = create_node(AST_WHILE);
    node->left = condition;
    node->right = body;
    return node;
}

static ASTNode *parse_repeat_statement(void) {
    advance(); // consume 'repeat'
    ASTNode *body = parse_block();
    expect(TOKEN_UNTIL);
    expect(TOKEN_LPAREN);
    ASTNode *condition = parse_bool();
    expect(TOKEN_RPAREN);

    ASTNode *node = create_node(AST_REPEAT);
    node->left = body;
    node->right = condition;
    return node;
}

static ASTNode *parse_print_statement(void) {
    advance(); // consume 'print'
    ASTNode *expr = parse_bool();
    expect(TOKEN_SEMICOLON);

    ASTNode *node = create_node(AST_PRINT);
    node->left = expr;
    return node;
}

static ASTNode *parse_block(void) {
    // printf("test");
    expect(TOKEN_LBRACE);

    ASTNode *block = create_node(AST_BLOCK);
    ASTNode *current = block;

    while (!match(TOKEN_RBRACE) && current_token.type != TOKEN_EOF) {
        current->left = parse_statement();
        if (!match(TOKEN_RBRACE)) {
            current->right = create_node(AST_BLOCK);
            current = current->right;
        }
    }

    expect(TOKEN_RBRACE);
    return block;
}

static ASTNode *parse_factorial(void) {
    advance(); // consume '!'
    expect(TOKEN_LPAREN);
    ASTNode *arg = parse_bool();
    expect(TOKEN_RPAREN);
    expect(TOKEN_SEMICOLON);

    ASTNode *node = create_node(AST_FACTORIAL);
    node->left = arg;
    return node;
}

static ASTNode *parse_parameters(void) {
    if (!match(TOKEN_INT)) {
        parse_error(PARSE_ERROR_UNEXPECTED_TOKEN, current_token);
        error_recovery();
        return NULL;
    }

    ASTNode *param = create_node(AST_PARAM);
    param->token = current_token; // store the type (int)
    advance(); // consume type

    // Handle parameter name
    if (!match(TOKEN_IDENTIFIER)) {
        parse_error(PARSE_ERROR_MISSING_IDENTIFIER, current_token);
        error_recovery();
        return NULL;
    }

    // Create identifier node for parameter name
    ASTNode *identifier = create_node(AST_IDENTIFIER);
    identifier->token = current_token;
    param->left = identifier;


    advance(); // consume identifier
    return param;
}

static ASTNode *parse_functions(void) {
    ASTNode *node = create_node(AST_FUNCDECL);
    advance(); // consume 'int' or whatever return type

    if (!match(TOKEN_IDENTIFIER)) {
        parse_error(PARSE_ERROR_MISSING_IDENTIFIER, current_token);
        error_recovery();
        return NULL;
    }

    // Store function name
    node->token = current_token;
    advance(); // consume function name

    // Parse parameters
    if (!match(TOKEN_LPAREN)) {
        parse_error(PARSE_ERROR_MISSING_PARENTHESIS, current_token);
        error_recovery();
        return NULL;
    }
    advance(); // consume '('

    ASTNode *parameter_list = NULL;
    ASTNode *current_param = NULL;

    // Parse parameter list until ')' is reachedd
    if (!match(TOKEN_RPAREN)) {
        parameter_list = create_node(AST_BLOCK); // Use block type for parameter list
        current_param = parameter_list;

        do {
            current_param->left = parse_parameters();

            // Check if there are more parameters and create new node fo next parameter
            if (match(TOKEN_COMMA)) {
                advance(); // Consume comma
                current_param->right = create_node(AST_BLOCK);
                current_param = current_param->right;
            }
        } while (match(TOKEN_INT) && !match(TOKEN_EOF));
    }

    if (!match(TOKEN_RPAREN)) {
        parse_error(PARSE_ERROR_MISSING_PARENTHESIS, current_token);
        error_recovery();
        return NULL;
    }
    advance(); // consume ')'
    node->right = parameter_list;

    // Parse function body
    if (match(TOKEN_LBRACE)) {
        // Must access to parameters in body as they're in the same or parent scope
        node->left = parse_block();
    } else {
        parse_error(PARSE_ERROR_MISSING_BLOCK_BRACES, current_token);
        error_recovery();
        return NULL;
    }

    return node;
}

//------------------------------------------------------------------------------------------------------------------------Added code above

// Parse variable declaration: int x;
static ASTNode *parse_declaration(void) {
    ASTNode *node = create_node(AST_VARDECL);
    advance(); // consume 'int'

    if (!match(TOKEN_IDENTIFIER)) {
        parse_error(PARSE_ERROR_MISSING_IDENTIFIER, current_token);
        error_recovery();
        return NULL;
    }

    node->token = current_token;
    advance();

    if (!match(TOKEN_SEMICOLON)) {
        parse_error(PARSE_ERROR_MISSING_SEMICOLON, current_token);
        error_recovery();
        return NULL;
    }
    advance();
    return node;
}

// Parse assignment: x = 5;
static ASTNode *parse_assignment(void) {
    ASTNode *node = create_node(AST_ASSIGN);
    node->left = create_node(AST_IDENTIFIER);
    node->left->token = current_token;

    advance();
    if (!match(TOKEN_ASSIGN)) {
        parse_error(PARSE_ERROR_MISSING_EQUALS, current_token);
        error_recovery();
        return NULL;
    }

    advance();
    node->right = parse_bool();

    if (!match(TOKEN_SEMICOLON)) {
        parse_error(PARSE_ERROR_MISSING_SEMICOLON, current_token);
        error_recovery();
        return NULL;
    }

    advance();
    return node;
}

// Parse statement
static ASTNode *parse_statement(void) {
    // if (match(TOKEN_INT)) {
    //     return parse_declaration();
    if (match(TOKEN_INT)) {
        // Need to "peak" here to see if this is a variable or function declaration, thus save state variables to be reloaded positions later
        Token saved_token = current_token;
        int saved_position = position;

        advance(); // consume 'int'

        if (match(TOKEN_IDENTIFIER)) {
            advance(); // consume identifier

            if (match(TOKEN_LPAREN)) {
                current_token = saved_token;
                position = saved_position;
                return parse_functions();
            } else {
                current_token = saved_token;
                position = saved_position;
                return parse_declaration();
            }
        } else {
            current_token = saved_token;
            position = saved_position;
            return parse_declaration(); // continue but invalid identifier
        }
    } else if (match(TOKEN_IDENTIFIER)) {
        return parse_assignment();
    }
    // TODO 4: Add cases for new statement types
    // else if (match(TOKEN_IF)) return parse_if_statement();
    // else if (match(TOKEN_WHILE)) return parse_while_statement();
    // else if (match(TOKEN_REPEAT)) return parse_repeat_statement();
    // else if (match(TOKEN_PRINT)) return parse_print_statement();
    // ...
    //------------------------------------------------------------------------------------------------------------------------Added code under
    else if (match(TOKEN_IF)) {
        return parse_if_statement();
    } else if (match(TOKEN_WHILE)) {
        return parse_while_statement();
    } else if (match(TOKEN_REPEAT)) {
        return parse_repeat_statement();
    } else if (match(TOKEN_PRINT)) {
        return parse_print_statement();
    } else if (match(TOKEN_FACTORIAL)) {
        return parse_factorial();
    } else if (match(TOKEN_LBRACE)) {
        return parse_block();
    } else {
        parse_error(PARSE_ERROR_UNEXPECTED_TOKEN, current_token);
        error_recovery();
        return NULL;
    }
}

// Parse expression (currently only handles numbers and identifiers)

// TODO 5: Implement expression parsing
// Current expression parsing is basic. Need to implement:
// - Binary operations (+-*/)
// - Comparison operators (<, >, ==, etc.)
// - Operator precedence
// - Parentheses grouping
// - Function calls
//------------------------------------------------------------------------------------------------------------------------Added code below

static ASTNode *parse_primary(void) {
    if (match(TOKEN_NUMBER)) {
        ASTNode *node = create_node(AST_NUMBER);
        advance();
        return node;
    } else if (match(TOKEN_IDENTIFIER)) {
        ASTNode *node = create_node(AST_IDENTIFIER);
        advance();
        return node;
    } else if (match(TOKEN_LPAREN)) {
        advance();
        ASTNode *expr = parse_bool();
        expect(TOKEN_RPAREN);
        return expr;
    } else {
        parse_error(PARSE_ERROR_INVALID_EXPRESSION, current_token);
        error_recovery();
        return NULL;
    }
}

// Handles OR boolean
static ASTNode *parse_bool(void) {
    ASTNode *node = parse_join();
    while (match(TOKEN_OR)) {
        ASTNode *opNode = create_node(AST_BOOLOP);
        opNode->token = current_token;
        advance();
        opNode->left = node;
        opNode->right = parse_join();
        node = opNode;
    }
    return node;
}

// Handles AND boolean
static ASTNode *parse_join(void) {
    ASTNode *node = parse_equality();
    while (match(TOKEN_AND)) {
        ASTNode *opNode = create_node(AST_BOOLOP);
        opNode->token = current_token;
        advance();
        opNode->left = node;
        opNode->right = parse_equality();
        node = opNode;
    }
    return node;
}

// Handles == and !=
static ASTNode *parse_equality(void) {
    ASTNode *node = parse_relational();
    while (match(TOKEN_EQ) || match(TOKEN_NEQ)) {
        ASTNode *opNode = create_node(AST_COMPARISONOP);
        opNode->token = current_token;
        advance();
        opNode->left = node;
        opNode->right = parse_relational();
        node = opNode;
    }
    return node;
}

// Handles > and <
static ASTNode *parse_relational(void) {
    ASTNode *node = parse_expression();
    while (match(TOKEN_LT) || match(TOKEN_GT)) {
        ASTNode *opNode = create_node(AST_COMPARISONOP);
        opNode->token = current_token;
        advance();
        opNode->left = node;
        opNode->right = parse_expression();
        node = opNode;
    }
    return node;
}

// Handles + and -
static ASTNode *parse_expression(void) {
    ASTNode *node = parse_term();
    while (match(TOKEN_PLUS) || match(TOKEN_MINUS)) {
        ASTNode *opNode = create_node(AST_BINOP);
        opNode->token = current_token;
        advance();
        opNode->left = node;
        opNode->right = parse_term();
        node = opNode;
    }
    return node;
}

// Handles * and /
static ASTNode *parse_term(void) {
    ASTNode *node = parse_unary();
    while (match(TOKEN_STAR) || match(TOKEN_SLASH)) {
        ASTNode *opNode = create_node(AST_BINOP);
        opNode->token = current_token;
        advance();
        opNode->left = node;
        opNode->right = parse_unary();
        node = opNode;
    }
    return node;
}

// Handles factorial
static ASTNode *parse_unary(void) {
    if (match(TOKEN_FACTORIAL)) {
        ASTNode *opNode = create_node(AST_FACTORIAL);
        opNode->token = current_token;
        advance();
        opNode->right = parse_primary();
        return opNode;
    } else if (match(TOKEN_ADDRESS)) {
        ASTNode *opNode = create_node(AST_ADDRESS_OF);
        opNode->token = current_token;
        advance();
        opNode->right = parse_primary();
        return opNode;
    }
    return parse_primary();
}

//------------------------------------------------------------------------------------------------------------------------Added code above

// Parse program (multiple statements)
static ASTNode *parse_program(void) {
    ASTNode *program = create_node(AST_PROGRAM);
    ASTNode *current = program;

    while (!match(TOKEN_EOF)) {
        current->left = parse_statement();
        if (!match(TOKEN_EOF)) {
            current->right = create_node(AST_PROGRAM);
            current = current->right;
        }
    }

    return program;
}

// Initialize parser
void parser_init(const char *input) {
    source = input;
    position = 0;
    advance(); // Get first token
}

// Main parse function
ASTNode *parse(void) {
    return parse_program();
}

// Print AST (for debugging)
void print_ast(ASTNode *node, int level) {
    if (!node) return;

    // Indent based on level
    for (int i = 0; i < level; i++) printf("  ");

    // Print node info
    switch (node->type) {
        case AST_PROGRAM:
            printf("Program\n");
            break;
        case AST_VARDECL:
            printf("VarDecl: %s\n", node->token.lexeme);
            break;
        case AST_ASSIGN:
            printf("Assign\n");
            break;
        case AST_NUMBER:
            printf("Number: %s\n", node->token.lexeme);
            break;
        case AST_IDENTIFIER:
            printf("Identifier: %s\n", node->token.lexeme);
            break;
        // TODO 6: Add cases for new node types
        // case AST_IF: printf("If\n"); break;
        // case AST_WHILE: printf("While\n"); break;
        // case AST_REPEAT: printf("Repeat-Until\n"); break;
        // case AST_BLOCK: printf("Block\n"); break;
        // case AST_BINOP: printf("BinaryOp: %s\n", node->token.lexeme); break;
        //------------------------------------------------------------------------------------------------------------------------Added code under
        case AST_IF:
            printf("If\n");
            break;
        case AST_WHILE:
            printf("While\n");
            break;
        case AST_REPEAT:
            printf("RepeatUntil\n");
            break;
        case AST_BLOCK:
            printf("Block\n");
            break;
        case AST_PRINT:
            printf("Print\n");
            break;
        case AST_FACTORIAL:
            printf("Factorial\n");
            break;
        case AST_BINOP:
            printf("BinaryOperation: %s\n", node->token.lexeme);
            break;
        case AST_COMPARISONOP:
            printf("ComaprisonOperation: %s\n", node->token.lexeme);
            break;
        case AST_BOOLOP:
            printf("BooleanOperation: %s\n", node->token.lexeme);
            break;
        case AST_FUNCDECL:
            printf("FunctionDeclare: %s\n", node->token.lexeme);
            break;
        case AST_PARAM:
            printf("FunctionParameter: %s\n", node->token.lexeme);
            break;
        //------------------------------------------------------------------------------------------------------------------------Added code above
        default:
            printf("Unknown AST node type\n");
    }

    // Print children
    print_ast(node->left, level + 1);
    print_ast(node->right, level + 1);
}

// Free AST memory
void free_ast(ASTNode *node) {
    if (!node) return;
    free_ast(node->left);
    free_ast(node->right);
    free(node);
}

// Example of examining tokens
void print_token_stream(const char *input) {
    int position = 0;
    Token token;

    do {
        token = get_next_token(input, &position);
        print_token(token);
    } while (token.type != TOKEN_EOF);
}

// Main function for testing
//int main() {
//    // Test with both valid and invalid inputs
//    const char *input = "int x;\n" // Valid declaration
//            "x = 42;\n"; // Valid assignment;
//    // TODO 8: Add more test cases and read from a file:
//    const char *invalid_input = "\n{"
//        "int var2;\n"
//        "int var6;\n"
//        "int otherVar;\n"
//        "otherVar = var2 + var6;"
//        "int anotherVar;\n"
//        "if (!27 && var2 < (27 + var6 > otherVar) || (anotherVar == 16) && var2 != 28 || 28 * 56 + 45 / 7 - 8) {}\n"
//        "int main(int sum, int a, int b) {\nsum = a + b;\n}\n"
//        "}"
//    ;
//
//    printf("Parsing input:\n%s\n", invalid_input);
//    parser_init(invalid_input);
//    ASTNode *ast = parse();
//
//    printf("\nAbstract Syntax Tree:\n");
//    print_ast(ast, 0);
//
//    free_ast(ast);
//    return 0;
//}
